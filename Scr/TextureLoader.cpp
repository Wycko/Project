#include "TextureLoader.h"


TextureLoader::TextureLoader()
{
}


TextureLoader::~TextureLoader()
{
	if( m_Texture )
	{
		//m_Texture->Release();
		//m_Texture = nullptr;
	}

	if( m_ResourceView )
	{
		//m_ResourceView->Release();
		//m_ResourceView = nullptr;
	}
}

bool TextureLoader::LoadDDSfromFile( ID3D11Device* Device, const LPCWSTR Filename )
{
	HRESULT hr;
	hr = DirectX::CreateDDSTextureFromFile( Device, Filename, m_Texture.GetAddressOf(), m_ResourceView.GetAddressOf() );
	if( FAILED( hr ) )
		return false;

	D3D11_RESOURCE_DIMENSION resType = D3D11_RESOURCE_DIMENSION_UNKNOWN;
	m_Texture->GetType( &resType );

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	m_Texture.As( &tex );
	//DX::ThrowIfFailed( hr );

	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc( &desc );

	return true;
}

ID3D11ShaderResourceView* TextureLoader::GetTexture() const
{
	return m_ResourceView.Get();
}
