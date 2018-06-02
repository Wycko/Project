#ifndef _BASICMODEL_H_
#define _BASICMODEL_H_

#include "Common.h"
#include "TextureLoader.h"


class BasicModel
{
public:
	BasicModel();
	~BasicModel();

	struct VERTEX 
	{
		DirectX::XMFLOAT3 Position; 
		DirectX::XMFLOAT2 tex;
	};

	bool Create( ID3D11Device* Device );
	void Render( ID3D11DeviceContext* DeviceContext );

private:

	ID3D11Buffer *pVBuffer = nullptr;
	TextureLoader m_Texture;
};


#endif // !_BASICMODEL_H_
