#ifndef _VERTEXTYPE_H_
#define _VERTEXTYPE_H_

#include "Common.h"


struct Vertex_PosCol
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

struct Vertex_PosTex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureUV;
};

struct Vertex_PosTexNor
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureUV;
	DirectX::XMFLOAT3 Normal;
};

struct Vertex_PosTexCol
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 TextureUV;
	DirectX::XMFLOAT4 Color;
};


#endif // !_VERTEXTYPE_H_
