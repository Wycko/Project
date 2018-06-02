#include "BasicModel.h"



BasicModel::BasicModel()
{
}


BasicModel::~BasicModel()
{
	if( pVBuffer )
	{
		pVBuffer->Release();
		pVBuffer = nullptr;
	}
}


bool BasicModel::Create( ID3D11Device* Device )
{
	HRESULT hr;
	
	// create a quad
	VERTEX OurVertices[] =
	{
		{ DirectX::XMFLOAT3( -0.5f, -0.5f, 0.0f ), DirectX::XMFLOAT2( 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -0.5f, 0.5f, 0.0f ), DirectX::XMFLOAT2( 0.0f, 0.0f ) },
		{ DirectX::XMFLOAT3( 0.5f, -0.5f, 0.0f ), DirectX::XMFLOAT2( 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( 0.5f, 0.5f, 0.0f ), DirectX::XMFLOAT2( 1.0f, 0.0f ) }
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof( bd ) );

	bd.Usage = D3D11_USAGE_DEFAULT;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof( OurVertices );             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;

	vertexData.pSysMem = OurVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = Device->CreateBuffer( &bd, &vertexData, &pVBuffer );	// create the buffer
	if( FAILED( hr ) )
		return false;

	if( !m_Texture.LoadDDSfromFile( Device, L"stone.dds" ) )
		return false;

	return true;
}


void BasicModel::Render( ID3D11DeviceContext * DeviceContext )
{
	UINT stride = sizeof( VERTEX );
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers( 0, 1, &pVBuffer, &stride, &offset );

	// select which primtive type we are using
	DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	ID3D11ShaderResourceView* tex = m_Texture.GetTexture();
	DeviceContext->PSSetShaderResources( 0, 1, &tex );
}
