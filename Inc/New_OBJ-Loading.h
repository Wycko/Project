#ifndef _OBJ_L_H_
#define _OBJ_L_H

#include "Common.h"
#include "OBJ-Loader.h"
#include <vector>


struct VertexData
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureUV;
};


class MeshMaster
{
public:
	bool CreateMesh( ID3D11Device* Device, const char* Filename );
	ID3D11Buffer** GetBuffer( UINT Index = 0 );

private:
	std::vector< Microsoft::WRL::ComPtr< ID3D11Buffer > > m_LoadedMeshs;
};


class ModelBase
{
public:
	ModelBase( DirectX::XMFLOAT3 Position, DirectX::XMFLOAT3 Rotation );
	~ModelBase();

	void SetPosition( DirectX::XMFLOAT3 Position );
	DirectX::XMFLOAT3 GetPosition();

	void SetRotation( DirectX::XMFLOAT3 Roation );
	DirectX::XMFLOAT3 GetRotation();

private:
	struct SubModel
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 BaseRotation;

		UINT NumberOfVertices;
		ID3D11Buffer* Mesh;
		ID3D11ShaderResourceView* Texture;
	};

	DirectX::XMFLOAT3 m_BasePosition;
	DirectX::XMFLOAT3 m_BaseRotation;
};


class StaticModel : public ModelBase
{
public:
	StaticModel( DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 Rotation = { 0.0f, 0.0f, 0.0f } );
	~StaticModel();

	void Render();
};


class ShaderBase
{
public:
	ShaderBase( ID3D11Device* Device );
	ShaderBase();

	bool CreateVertexAndPixelShader( ID3D11Device* Device );

private:
	Microsoft::WRL::ComPtr< ID3D11VertexShader > m_VertexShader;
	Microsoft::WRL::ComPtr< ID3D11PixelShader > m_PixelShader;

};

#endif // !_OBJ_L_H_
