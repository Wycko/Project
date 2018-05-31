////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <fstream>
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureloader.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FontClass
////////////////////////////////////////////////////////////////////////////////
class FontClass
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

public:
	FontClass();
	~FontClass();

	bool Initialize( ID3D11Device*, char*, WCHAR* );
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray( void*, char*, float, float );

private:
	bool LoadFontData( char* );
	void ReleaseFontData();
	bool LoadTexture( ID3D11Device*, WCHAR* );
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureLoader* m_Texture = nullptr;
};

#endif