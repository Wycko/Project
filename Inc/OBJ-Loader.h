#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include "Common.h"
#include <vector>

class OBJLoader
{
public:
	OBJLoader();
	~OBJLoader();

	struct VertexData
	{
		DirectX::XMFLOAT3 Vertex;
		DirectX::XMFLOAT2 Texture;
	};

	bool LoadGeometryFromOBJ( const char* Filename );
	VertexData* GetData()
	{
		return m_VertexBuffer.data();
	}
	UINT GetSize()
	{
		return ( UINT )( m_VertexBuffer.size() * sizeof( VertexData ) );
	}


private:
	struct FaceData
	{
		UINT Vertex;
		UINT UV;
		UINT Normal;
	};

	struct VertexDataN
	{
		DirectX::XMFLOAT3 Vertex;
		DirectX::XMFLOAT2 Texture;
		DirectX::XMFLOAT3 Normal;
	};

	

	void BuildFormData();

	std::vector< DirectX::XMFLOAT3 >	m_Vertices;
	std::vector< FaceData* >		m_Faces;	// X = Vertex, Y = UV, Z = Normal
	std::vector< DirectX::XMFLOAT2 >	m_TextureUV;
	std::vector< DirectX::XMFLOAT3 >	m_Normals;

	std::vector< VertexData > m_VertexBuffer;
	std::vector< short > m_IndexBuffer;

};


#endif // !_OBJ_LOADER_H_
