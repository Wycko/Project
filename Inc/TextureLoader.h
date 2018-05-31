#ifndef _TEXTURELOADER_H_
#define _TEXTURELOADER_H_

#include "Common.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>


class TextureLoader
{
public:
	TextureLoader();
	~TextureLoader();

	bool LoadDDSfromFile( ID3D11Device* Device, const LPCWSTR Filename );
	ID3D11ShaderResourceView* GetTexture() const;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ResourceView;
	ID3D11ShaderResourceView* m_ResourceView2 = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_Texture;
};


#endif // !_TEXTURELOADER_H_
