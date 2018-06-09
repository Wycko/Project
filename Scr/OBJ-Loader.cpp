#include "OBJ-Loader.h"
#include <fstream>


OBJLoader::OBJLoader()
{
}


OBJLoader::~OBJLoader()
{
}


bool OBJLoader::LoadGeometryFromOBJ( const char* Filename )
{
	char Buffer[ MAX_PATH ] = { 0 };
	char MTL_Filename[ MAX_PATH ] = { 0 };

	std::ifstream ObjFile;

	// Create temporary storage for the input data.
	//std::vector< Vec3 > VertexData;
	//std::vector< Vec3 > NormalData;
	//std::vector< Vec2 > TexCoordData;

	DWORD index = 0;
	DWORD Subset = 0;

	ObjFile.open( Filename );
	if( !ObjFile.good() )
		return false;

	while( ObjFile >> Buffer )
	{
		//ObjFile >> Buffer;

		if( 0 == strcmp( Buffer, "#" ) )
		{
			// Comment.
			ObjFile.ignore( 1000, '\n' );
			continue;
		}

		else if( 0 == strcmp( Buffer, "mtllib" ) )
		{
			// Material library.
			ObjFile >> MTL_Filename;
		}

		else if( 0 == strcmp( Buffer, "v" ) )
		{
			// Vertex position.
			float x, y, z;
			ObjFile >> x >> y >> z;
			m_Vertices.push_back( DirectX::XMFLOAT3( x, y, z ) );
		}

		else if( 0 == strcmp( Buffer, "vt" ) )
		{
			// Vertex TexCoord.
			float u, v;
			ObjFile >> u >> v;
			m_TextureUV.push_back( DirectX::XMFLOAT2( u, v ) );
		}

		else if( 0 == strcmp( Buffer, "vn" ) )
		{
			// Vertex Normal
			float x, y, z;
			ObjFile >> x >> y >> z;
			m_Normals.push_back( DirectX::XMFLOAT3( x, y, z ) );
		}

		else if( 0 == strcmp( Buffer, "f" ) )
		{
			m_Faces.push_back( new FaceData[ 3 ] );
			// Face
			UINT iPosition, iTexCoord, iNormal;

			for( UINT iFace = 0; iFace < 3; iFace++ )
			{
				FaceData& PerVertex = m_Faces.back()[ iFace ];
				// OBJ format uses 1-based arrays
				ObjFile >> iPosition;
				PerVertex.Vertex = iPosition - 1;

				if( '/' == ObjFile.peek() )
				{
					ObjFile.ignore();

					if( '/' != ObjFile.peek() )
					{
						// Optional texture coordinate
						ObjFile >> iTexCoord;
						PerVertex.UV = iTexCoord - 1;
					}

					if( '/' == ObjFile.peek() )
					{
						ObjFile.ignore();

						// Optional vertex normal
						ObjFile >> iNormal;
						PerVertex.Normal = iNormal - 1;
					}
				}
			}
			
			//m_Attributes.push_back( Subset );
		}

		//else if( 0 == strcmp( Buffer, "usemtl" ) )
		//{
		//	// Material
		//	char MtlName[ 256 ] = { 0 };
		//	ObjFile >> MtlName;

		//	bool Found = false;
		//	for( int Mat_Index = 0; Mat_Index < ( int )m_Material.size(); ++Mat_Index )
		//	{
		//		Material CurMat = m_Material[ Mat_Index ];
		//		if( 0 == strcmp( CurMat.strName, MtlName ) )
		//		{
		//			Found = true;
		//			Subset = Mat_Index;

		//			break;
		//		}
		//	}

		//	if( !Found )
		//	{
		//		Material material;

		//		InitializeMaterial( material );
		//		strcpy_s( material.strName, MtlName );

		//		Subset = m_Material.size();

		//		m_Material.push_back( material );
		//	}
		//}
	}

	ObjFile.close();

	BuildFormData();

	return true;
}


void OBJLoader::BuildFormData()
{
	short NumIndices = m_Faces.size() * 3;

	VertexData Vertex;

	for( auto it = m_Faces.begin(); it != m_Faces.end(); ++it )
	{
		for( int i = 0; i < 3; i++ )
		{			
			auto Index = ( *it )[ i ].Vertex;
			Vertex.Vertex = m_Vertices[ Index ];

			//Index = ( *it )[ i ].UV;
			//Vertex.Texture = m_TextureUV[ Index ];
			Vertex.Texture = { 0.5f,0.5f };

			m_VertexBuffer.push_back( Vertex );
		}
	}
}
