#ifndef _BASICSHADER_H_
#define _BASICSHADER_H_

#include "Common.h"

class BasicShader
{
public:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	BasicShader();
	~BasicShader();

	bool Create( ID3D11Device* Device, HWND hWnd );
	void Render( ID3D11DeviceContext* DeviceContext, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj );

private:

	ID3D11InputLayout *pLayout = nullptr;            // the pointer to the input layout
	ID3D11VertexShader *pVS = nullptr;               // the pointer to the vertex shader
	ID3D11PixelShader *pPS = nullptr;                // the pointer to the pixel shader
	ID3D11Buffer* m_matrixBuffer = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;
};


#endif // !_BASICSHADER_H_
