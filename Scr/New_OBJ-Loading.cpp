#include "New_OBJ-Loading.h"


bool MeshMaster::CreateMesh( ID3D11Device* Device, const char* Filename )
{
	OBJLoader m_Loader;
	if( !m_Loader.LoadGeometryFromOBJ( Filename ) )
		return false;

	HRESULT hr;
	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof( bd ) );

	bd.Usage = D3D11_USAGE_DEFAULT;                // write access access by CPU and GPU
	bd.ByteWidth = m_Loader.GetSize();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_Loader.GetData();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	Microsoft::WRL::ComPtr< ID3D11Buffer > pVBuffer;
	hr = Device->CreateBuffer( &bd, &vertexData, pVBuffer.GetAddressOf() );	// create the buffer
	if( FAILED( hr ) )
		return false;

	m_LoadedMeshs.push_back( pVBuffer );

	return true;
}


ID3D11Buffer** MeshMaster::GetBuffer( UINT Index )
{
	assert( Index >= 0 && Index < m_LoadedMeshs.size() );
	return m_LoadedMeshs[ Index ].GetAddressOf();
}
