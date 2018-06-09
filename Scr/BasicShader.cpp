#include "BasicShader.h"


BasicShader::BasicShader()
{
}


BasicShader::~BasicShader()
{
	if( pLayout )
	{
		pLayout->Release();
		pLayout = nullptr;
	}
	if( pVS )
	{
		pVS->Release();
		pVS = nullptr;
	}
	if( pPS )
	{
		pPS->Release();
		pPS = nullptr;
	}
	if( m_matrixBuffer )
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}
	if( m_sampleState )
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}
}


bool BasicShader::Create( ID3D11Device* Device, HWND hWnd )
{
	HRESULT hr;
	ID3D10Blob* errorMessage = nullptr;
	// load and compile the two shaders
	ID3D10Blob *PS = nullptr, *VS = nullptr;

	auto CompilerError = []( const HWND hWnd, ID3D10Blob* Error )
	{
		const char *src = (char*)Error->GetBufferPointer();
		size_t size = Error->GetBufferSize() + 1;
		wchar_t* dest = new wchar_t[size];

		size_t outSize;
		mbstowcs_s( &outSize, dest, size, src, size - 1 );

		MessageBox( hWnd, dest, L"Shader Compile Error", MB_OK );
		delete[] dest;
		dest = nullptr;
		src = nullptr;
	};

	hr = D3DCompileFromFile( L"shaders.shader", 0, 0, "VShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &VS, &errorMessage );
	if( FAILED( hr ) )
	{
		if( errorMessage )
		{
			CompilerError( hWnd, errorMessage );

			errorMessage->Release();
			errorMessage = nullptr;
		}
			return false;
	}

	// encapsulate both shaders into shader objects
	hr = Device->CreateVertexShader( VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS );
	if( FAILED( hr ) )
		return false;

	hr = D3DCompileFromFile( L"shaders.shader", 0, 0, "PShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &PS, &errorMessage );
	if( FAILED( hr ) )
	{
		if( errorMessage )
		{
			CompilerError( hWnd, errorMessage );

			errorMessage->Release();
			errorMessage = nullptr;
		}
			return false;
	}

	hr = Device->CreatePixelShader( PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS );
	if( FAILED( hr ) )
		return false;

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = Device->CreateInputLayout( ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout );

	VS->Release();
	VS = nullptr;
	PS->Release();
	PS = nullptr;

	if( FAILED( hr ) )
		return false;

	D3D11_BUFFER_DESC matrixBufferDesc;
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof( MatrixBufferType );
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = Device->CreateBuffer( &matrixBufferDesc, NULL, &m_matrixBuffer );
	if( FAILED( hr ) )
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = Device->CreateSamplerState( &samplerDesc, &m_sampleState );
	if( FAILED( hr ) )
	{
		return false;
	}

	return true;
}


void BasicShader::Render( ID3D11DeviceContext* DeviceContext, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj )
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr = nullptr;
	unsigned int bufferNumber;
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	worldMatrix = DirectX::XMMatrixIdentity();
	viewMatrix = DirectX::XMLoadFloat4x4( &view );
	projectionMatrix = DirectX::XMLoadFloat4x4( &proj );

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose( worldMatrix );
	viewMatrix = XMMatrixTranspose( viewMatrix );
	projectionMatrix = XMMatrixTranspose( projectionMatrix );

	// Lock the constant buffer so it can be written to.
	result = DeviceContext->Map( m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	if( FAILED( result ) )
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	DeviceContext->Unmap( m_matrixBuffer, 0 );

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	DeviceContext->VSSetConstantBuffers( bufferNumber, 1, &m_matrixBuffer );

	DeviceContext->PSSetSamplers( 0, 1, &m_sampleState );

	// set the shader objects
	DeviceContext->VSSetShader( pVS, 0, 0 );
	DeviceContext->PSSetShader( pPS, 0, 0 );
	DeviceContext->IASetInputLayout( pLayout );

	// draw the vertex buffer to the back buffer
	DeviceContext->Draw( 3614*3, 0 );
}
