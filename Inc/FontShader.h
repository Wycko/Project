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
#include "Timer.h"


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

	bool Initialize( ID3D11Device*, const char*, const WCHAR* );
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray( void*, const char*, float, float );

private:
	bool LoadFontData( const char* );
	void ReleaseFontData();
	bool LoadTexture( ID3D11Device*, const WCHAR* );
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureLoader* m_Texture = nullptr;

	Timer m_Time;
};


#endif //!_FONTCLASS_H_
