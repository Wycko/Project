#include "MyText.h"

#include <fstream>
#include <DDSTextureLoader.h>


MyText::MyText()
{
}


MyText::~MyText()
{
	m_CharacterSet.clear();
}


bool MyText::Initialize( ID3D11Device* Device, DirectX::XMFLOAT4X4 BaseViewMatrix )
{
	m_BaseViewMatrix = BaseViewMatrix;

	if( !LoadFontData( Device, "fontdata.txt", L"font.dds" ) )
		return false;

	if( !CreateVertexAndIndexBuffer( Device ) )
		return false;

	if( !CreateShadersAndInpuLayout( Device, L"shaders.shader" ) )
		return false;

	GrowQueue();
	GrowSprite();

	return true;
}


void MyText::AddText( const char* Text, DirectX::XMFLOAT2 Position )
{
	m_Time.Start();

	float drawX = ( float )( ( ( 800 / 2 ) * -1 ) + Position.x );
	float drawY = ( float )( ( 600 / 2 ) - Position.y );
	float offsetX = drawX;

	for( ; *Text; Text++ )
	{
		char character = *Text;

		switch( character )
		{
			case '\r':
				// Skip carriage returns.
				continue;

			case '\n':
				// New line.
				offsetX = drawX;
				drawY -= m_FontSize;
				break;

			case ' ':
				offsetX += 3.0f;
				break;

			default:
				if( m_QueueCount >= m_QueueSize )
					GrowQueue();

				// Output this character.
				//auto glyph = FindCharacter( character );

				//auto sym = m_CharacterSet.find( character );

				auto glyph = &m_FontData[ *Text - 32 ];

				auto Info = &m_SpriteQueue[ m_QueueCount ];

				// First triangle in quad.
				//Info->Position = DirectX::XMFLOAT3( offsetX, drawY, ( float )sym->second.Size );
				//Info->Texture = DirectX::XMFLOAT2( sym->second.offsetX, sym->second.offsetY );

				Info->Position = DirectX::XMFLOAT3( offsetX, drawY, ( float )glyph->Size );
				Info->Texture = DirectX::XMFLOAT2( glyph->Left, glyph->Right );

				m_QueueCount++;

				// Update the x location for drawing by the size of the letter and one pixel.
				offsetX += glyph->Size + 1.0f;
				//offsetX += sym->second.Size + 1.0f;
		}
	}
	int Z = (int)m_Time.GetElapsedTime();
	int a = Z;
}


void MyText::RenderText( ID3D11DeviceContext* DeviceContext, DirectX::XMFLOAT4X4 OrthographicMatrix )
{
	// Nothing to render
	if( m_QueueCount == 0 )
		return;

	PrepareForRendering( DeviceContext, OrthographicMatrix );

	/*if( m_SortedQueue.size() < m_QueueCount )
	{
		GrowSprite();
	}*/

	//RenderBatch( DeviceContext, &m_SortedQueue[ 0 ], m_QueueCount );

	RenderBatch2( DeviceContext, m_SpriteQueue.get(), m_QueueCount );

	m_QueueCount = 0;
	//m_SortedQueue.clear();
}


bool MyText::LoadFontData( ID3D11Device* Device, const char* Filename, const WCHAR* TextureName )
{
	HRESULT hr = S_OK;
	std::ifstream fin;
	int temp;

	// Read in the font size and spacing between chars.
	fin.open( Filename );
	if( fin.fail() )
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	while( fin >> temp )
	{
		CharaterData Data;

		fin >> Data.Character;
		fin >> Data.Left;
		fin >> Data.Right;
		fin >> Data.Size;

		/*Symbol Sym;
		Sym.offsetX = Data.Left;
		Sym.offsetY = Data.Right;
		Sym.Size = Data.Size;
		m_CharacterSet[ Data.Character ] = Sym;*/

		m_FontData.push_back( Data );
	}

	m_FontData.shrink_to_fit();

	m_DefaultCharacter = FindCharacter( '?' );

	hr = DirectX::CreateDDSTextureFromFile( Device, TextureName, NULL, m_FontTexture.GetAddressOf() );
	if( FAILED( hr ) )
		return false;

	return true;
}


bool MyText::CreateVertexAndIndexBuffer( ID3D11Device* Device )
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC vertexBufferDesc = {};

	vertexBufferDesc.ByteWidth		= sizeof( VertexData ) * m_MaxBatchSize * m_VerticesPerCharacter;
	vertexBufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	hr = Device->CreateBuffer( &vertexBufferDesc, nullptr, m_VertexBuffer.GetAddressOf() );
	if( FAILED( hr ) )
		return false;

	D3D11_BUFFER_DESC indexBufferDesc = {};

	//static_assert( ( MaxBatchSize * VerticesPerSprite ) < USHRT_MAX, "MaxBatchSize too large for 16-bit indices" );

	indexBufferDesc.ByteWidth	= sizeof( short ) * m_MaxBatchSize * m_IndicesPerCharacter;
	indexBufferDesc.BindFlags	= D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage		= D3D11_USAGE_DEFAULT;

	std::vector< short > indices;
	indices.reserve( m_MaxBatchSize * m_IndicesPerCharacter );

	for( short i = 0; i < m_MaxBatchSize * m_VerticesPerCharacter; i += m_VerticesPerCharacter )
	{
		indices.push_back( i );
		indices.push_back( i + 1 );
		indices.push_back( i + 2 );

		indices.push_back( i + 1 );
		indices.push_back( i + 3 );
		indices.push_back( i + 2 );
	}

	D3D11_SUBRESOURCE_DATA indexDataDesc = {};

	indexDataDesc.pSysMem = indices.data();

	hr = Device->CreateBuffer( &indexBufferDesc, &indexDataDesc, m_IndexBuffer.GetAddressOf() );
	if( FAILED( hr ) )
		return false;

	return true;
}


bool MyText::CreateShadersAndInpuLayout( ID3D11Device* Device, const WCHAR* Filename )
{
	HRESULT hr;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[ 2 ];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// Initialize the pointers this function will use to null.
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	hr = D3DCompileFromFile( Filename, NULL, NULL, "VShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, NULL );
	if( FAILED( hr ) )
	{
		if( vertexShaderBuffer )
			vertexShaderBuffer->Release();

		return false;
	}
		

	// Compile the pixel shader code.
	hr = D3DCompileFromFile( Filename, NULL, NULL, "TPShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, NULL );
	if( FAILED( hr ) )
	{
		if( vertexShaderBuffer )
			vertexShaderBuffer->Release();

		if( pixelShaderBuffer )
			pixelShaderBuffer->Release();

		return false;
	}

	// Create the vertex shader from the buffer.
	hr = Device->CreateVertexShader( vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL,	m_VertexShader.GetAddressOf() );
	if( FAILED( hr ) )
	{
		if( vertexShaderBuffer )
			vertexShaderBuffer->Release();

		if( pixelShaderBuffer )
			pixelShaderBuffer->Release();

		return false;
	}

	// Create the vertex shader from the buffer.
	hr = Device->CreatePixelShader( pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, m_PixelShader.GetAddressOf() );
	if( FAILED( hr ) )
	{
		if( vertexShaderBuffer )
			vertexShaderBuffer->Release();

		if( pixelShaderBuffer )
			pixelShaderBuffer->Release();

		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[ 0 ].SemanticName = "POSITION";
	polygonLayout[ 0 ].SemanticIndex = 0;
	polygonLayout[ 0 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[ 0 ].InputSlot = 0;
	polygonLayout[ 0 ].AlignedByteOffset = 0;
	polygonLayout[ 0 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[ 0 ].InstanceDataStepRate = 0;

	polygonLayout[ 1 ].SemanticName = "TEXCOORD";
	polygonLayout[ 1 ].SemanticIndex = 0;
	polygonLayout[ 1 ].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[ 1 ].InputSlot = 0;
	polygonLayout[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[ 1 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[ 1 ].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof( polygonLayout ) / sizeof( polygonLayout[ 0 ] );

	// Create the vertex input layout.
	hr = Device->CreateInputLayout( polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),	vertexShaderBuffer->GetBufferSize(), m_InputLayout.GetAddressOf() );
	if( FAILED( hr ) )
	{
		if( vertexShaderBuffer )
			vertexShaderBuffer->Release();

		if( pixelShaderBuffer )
			pixelShaderBuffer->Release();

		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic constant buffer that is in the vertex shader.
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof( ConstantBufferType );
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hr = Device->CreateBuffer( &constantBufferDesc, NULL, m_ConstantBuffer.GetAddressOf() );
	if( FAILED( hr ) )
		return false;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[ 0 ] = 0;
	samplerDesc.BorderColor[ 1 ] = 0;
	samplerDesc.BorderColor[ 2 ] = 0;
	samplerDesc.BorderColor[ 3 ] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = Device->CreateSamplerState( &samplerDesc, m_SampleState.GetAddressOf() );
	if( FAILED( hr ) )
		return false;

	return true;
}


void MyText::PrepareForRendering( ID3D11DeviceContext* DeviceContext, DirectX::XMFLOAT4X4 OrthographicMatrix )
{
	DeviceContext->PSSetSamplers( 0, 1, m_SampleState.GetAddressOf() );
	DeviceContext->PSSetShaderResources( 0, 1, m_FontTexture.GetAddressOf() );

	// Set shaders.
	DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	DeviceContext->IASetInputLayout( m_InputLayout.Get() );
	DeviceContext->VSSetShader( m_VertexShader.Get(), nullptr, 0 );
	DeviceContext->PSSetShader( m_PixelShader.Get(), nullptr, 0 );

	UINT vertexStride = sizeof( VertexData );
	UINT vertexOffset = 0;

	DeviceContext->IASetVertexBuffers( 0, 1, m_VertexBuffer.GetAddressOf(), &vertexStride, &vertexOffset );
	DeviceContext->IASetIndexBuffer( m_IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DeviceContext->Map( m_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );

	// Get a pointer to the data in the constant buffer.
	auto dataPtr = ( ConstantBufferType* )mappedResource.pData;

	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	worldMatrix = DirectX::XMMatrixIdentity();
	viewMatrix = DirectX::XMLoadFloat4x4( &m_BaseViewMatrix );
	projectionMatrix = DirectX::XMLoadFloat4x4( &OrthographicMatrix );

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose( worldMatrix );
	viewMatrix = XMMatrixTranspose( viewMatrix );
	projectionMatrix = XMMatrixTranspose( projectionMatrix );

	// Copy the matrices into the constant buffer.
	dataPtr->World = worldMatrix;
	dataPtr->View = viewMatrix;
	dataPtr->Projection = projectionMatrix;

	DeviceContext->Unmap( m_ConstantBuffer.Get(), 0 );

	DeviceContext->VSSetConstantBuffers( 0, 1, m_ConstantBuffer.GetAddressOf() );

	m_VertexBufferPosition = 0;
}


void MyText::RenderSprite( VertexData const* sprite, VertexData* vertices )
{
	int index = 0;

	vertices[ index ].Position = DirectX::XMFLOAT3( sprite->Position.x , sprite->Position.y, 0.0f );  // Top left.
	vertices[ index ].Texture = DirectX::XMFLOAT2( sprite->Texture.x, 0.0f );
	index++;

	vertices[ index ].Position = DirectX::XMFLOAT3( sprite->Position.x + sprite->Position.z, sprite->Position.y, 0.0f );  // Top right.
	vertices[ index ].Texture = DirectX::XMFLOAT2( sprite->Texture.y, 0.0f );
	index++;

	vertices[ index ].Position = DirectX::XMFLOAT3( sprite->Position.x, sprite->Position.y - m_FontSize, 0.0f );  // Bottom left.
	vertices[ index ].Texture = DirectX::XMFLOAT2( sprite->Texture.x, 1.0f );
	index++;

	vertices[ index ].Position = DirectX::XMFLOAT3( sprite->Position.x + sprite->Position.z, sprite->Position.y - m_FontSize, 0.0f );  // Bottom right.
	vertices[ index ].Texture = DirectX::XMFLOAT2( sprite->Texture.y, 1.0f );
	index++;
}


void MyText::RenderBatch( ID3D11DeviceContext* DeviceContext, VertexData const* const* sprites, size_t count )
{
	while( count > 0 )
	{
		// How many sprites do we want to draw?
		size_t batchSize = count;

		// How many sprites does the D3D vertex buffer have room for?
		size_t remainingSpace = m_MaxBatchSize - m_VertexBufferPosition;

		if( batchSize > remainingSpace )
		{
			if( remainingSpace < m_MinBatchSize )
			{
				// If we are out of room, or about to submit an excessively small batch, wrap back to the start of the vertex buffer.
				m_VertexBufferPosition = 0;

				batchSize = std::min( count, m_MaxBatchSize );
			}
			else
			{
				// Take however many sprites fit in what's left of the vertex buffer.
				batchSize = remainingSpace;
			}
		}

		// Lock the vertex buffer.
		D3D11_MAP mapType = ( m_VertexBufferPosition == 0 ) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;

		D3D11_MAPPED_SUBRESOURCE mappedBuffer;

		DeviceContext->Map( m_VertexBuffer.Get(), 0, mapType, 0, &mappedBuffer );

		auto vertices = static_cast< VertexData* >( mappedBuffer.pData ) + m_VertexBufferPosition * m_VerticesPerCharacter;

		// Generate sprite vertex data.
		for( size_t i = 0; i < batchSize; i++ )
		{
			//assert( i < count );
			RenderSprite( sprites[ i ], vertices );

			vertices += m_VerticesPerCharacter;
		}

		DeviceContext->Unmap( m_VertexBuffer.Get(), 0 );

		// Ok lads, the time has come for us draw ourselves some sprites!
		UINT startIndex = ( UINT )m_VertexBufferPosition * m_IndicesPerCharacter;
		UINT indexCount = ( UINT )batchSize * m_IndicesPerCharacter;

		DeviceContext->DrawIndexed( indexCount, startIndex, 0 );

		// Advance the buffer position.

		m_VertexBufferPosition += batchSize;
		
		sprites += batchSize;
		count -= batchSize;
	}
}


void MyText::RenderBatch2( ID3D11DeviceContext* DeviceContext, MyText::VertexData* Queue, size_t count )
{
	while( count > 0 )
	{
		// How many sprites do we want to draw?
		size_t batchSize = count;

		// How many sprites does the D3D vertex buffer have room for?
		size_t remainingSpace = m_MaxBatchSize - m_VertexBufferPosition;

		if( batchSize > remainingSpace )
		{
			if( remainingSpace < m_MinBatchSize )
			{
				// If we are out of room, or about to submit an excessively small batch, wrap back to the start of the vertex buffer.
				m_VertexBufferPosition = 0;

				batchSize = std::min( count, m_MaxBatchSize );
			}
			else
			{
				// Take however many sprites fit in what's left of the vertex buffer.
				batchSize = remainingSpace;
			}
		}

		// Lock the vertex buffer.
		D3D11_MAP mapType = ( m_VertexBufferPosition == 0 ) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;

		D3D11_MAPPED_SUBRESOURCE mappedBuffer;

		DeviceContext->Map( m_VertexBuffer.Get(), 0, mapType, 0, &mappedBuffer );

		auto vertices = static_cast< VertexData* >( mappedBuffer.pData ) + m_VertexBufferPosition * m_VerticesPerCharacter;

		// Generate sprite vertex data.
		for( size_t i = 0; i < batchSize; i++ )
		{
			//assert( i < count );
			RenderSprite( Queue + i, vertices );

			vertices += m_VerticesPerCharacter;
		}


		DeviceContext->Unmap( m_VertexBuffer.Get(), 0 );

		// Ok lads, the time has come for us draw ourselves some sprites!
		UINT startIndex = ( UINT )m_VertexBufferPosition * m_IndicesPerCharacter;
		UINT indexCount = ( UINT )batchSize * m_IndicesPerCharacter;

		DeviceContext->DrawIndexed( indexCount, startIndex, 0 );

		// Advance the buffer position.

		m_VertexBufferPosition += batchSize;

		Queue += batchSize;
		count -= batchSize;
	}
}


const MyText::CharaterData* MyText::FindCharacter( const WCHAR& Character )
{
	auto glyph = std::lower_bound( m_FontData.begin(), m_FontData.end(), Character );

	if( glyph != m_FontData.end() && glyph->Character == Character )
		return &*glyph;

	return m_DefaultCharacter;
}


void MyText::GrowQueue()
{
	// Grow by a factor of 2.
	size_t newSize = std::max( m_InitialQueueSize, m_QueueSize * 2 );

	// Allocate the new array.
	std::unique_ptr< VertexData[] > newArray( new VertexData[ newSize ] );

	// Copy over any existing sprites.
	for( size_t i = 0; i < m_QueueCount; i++ )
	{
		newArray[ i ] = m_SpriteQueue[ i ];
	}

	// Replace the previous array with the new one.
	m_SpriteQueue = std::move( newArray );
	m_QueueSize = newSize;

	m_SortedQueue.clear();
}


void MyText::GrowSprite()
{
	size_t previousSize = m_SortedQueue.size();

	m_SortedQueue.resize( m_QueueCount );

	for( size_t i = previousSize; i < m_QueueCount; i++ )
	{
		m_SortedQueue[ i ] = &m_SpriteQueue[ i ];
	}
}
