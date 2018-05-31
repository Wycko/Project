////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "font_class.h"
#include "fontshader.h"
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	~TextClass();

	bool Initialize( ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMFLOAT4X4 );
	void Shutdown();
	bool Render( ID3D11DeviceContext*, XMFLOAT4X4, XMFLOAT4X4, char* txt );

private:
	bool InitializeSentence( SentenceType**, int, ID3D11Device* );
	bool UpdateSentence( SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext* );
	void ReleaseSentence( SentenceType** );
	bool RenderSentence( ID3D11DeviceContext*, SentenceType*, XMFLOAT4X4, XMFLOAT4X4 );

private:
	FontClass* m_Font;
	FontShaderClass* m_FontShader;
	int m_screenWidth, m_screenHeight;
	XMFLOAT4X4 m_baseViewMatrix;
	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
};

#endif