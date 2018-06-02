#ifndef _MY_TEXT_H_
#define _MY_TEXT_H_

#include "Common.h"
#include <vector>
#include <algorithm>
#include <map>
#include "Timer.h"


class MyText
{
public:
	MyText();
	~MyText();

	bool Initialize( ID3D11Device* Device, DirectX::XMFLOAT4X4 BaseViewMatrix );
	void AddText( const char* Text, DirectX::XMFLOAT2 Position );
	void RenderText( ID3D11DeviceContext* DeviceContext, DirectX::XMFLOAT4X4 OrthographicMatrix );

private:
	struct CharaterData
	{
		bool operator< ( const CharaterData& right )
		{
			return Character < right.Character;
		}

		bool operator< ( const wchar_t& right )
		{
			return Character < right;
		}

		char Character;
		float Left;
		float Right;
		int Size;
	};

	struct VertexData
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
	};

	struct ConstantBufferType
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};


	////////////////////
	// TEST!
	////////////////////
	struct Symbol
	{
		float offsetX;
		float offsetY;
		float Size;
	};

	void RenderBatch2( ID3D11DeviceContext* DeviceContext, MyText::VertexData* Queue, size_t count );
	std::map< char, Symbol > m_CharacterSet;

	Timer m_Time;
	////////////////////

	bool LoadFontData( ID3D11Device* Device, const char* Filename, const WCHAR* TextureName );
	bool CreateVertexAndIndexBuffer( ID3D11Device* Device );
	bool CreateShadersAndInpuLayout( ID3D11Device* Device, const WCHAR* Filename );

	void PrepareForRendering( ID3D11DeviceContext* DeviceContext, DirectX::XMFLOAT4X4 OrthographicMatrix );
	void RenderBatch( ID3D11DeviceContext* DeviceContext, VertexData const* const* sprites, size_t count );
	void RenderSprite( VertexData const* sprite, VertexData* vertices );

	const CharaterData* FindCharacter( const WCHAR& Character );
	void GrowQueue();
	void GrowSprite();

	static const size_t m_MaxBatchSize			= 2048;
	static const size_t m_MinBatchSize			= 128;
	static const size_t m_InitialQueueSize		= 64;
	static const size_t m_VerticesPerCharacter	= 4;
	static const size_t m_IndicesPerCharacter	= 6;

	size_t m_VertexBufferPosition = 0;
	const float m_FontSize = 16.0f;

	Microsoft::WRL::ComPtr< ID3D11VertexShader >	m_VertexShader;
	Microsoft::WRL::ComPtr< ID3D11PixelShader >		m_PixelShader;
	Microsoft::WRL::ComPtr< ID3D11InputLayout >		m_InputLayout;
	Microsoft::WRL::ComPtr< ID3D11Buffer >			m_IndexBuffer;
	Microsoft::WRL::ComPtr< ID3D11Buffer >			m_VertexBuffer;
	Microsoft::WRL::ComPtr< ID3D11Buffer >			m_ConstantBuffer;
	Microsoft::WRL::ComPtr< ID3D11SamplerState >	m_SampleState;
	Microsoft::WRL::ComPtr< ID3D11ShaderResourceView > m_FontTexture;

	DirectX::XMFLOAT4X4 m_BaseViewMatrix;
	std::unique_ptr< VertexData[] > m_SpriteQueue;
	std::vector< const VertexData* > m_SortedQueue;
	std::vector< CharaterData > m_FontData;

	size_t m_QueueCount = 0;
	size_t m_QueueSize = 0;

	const CharaterData* m_DefaultCharacter = nullptr;
};


#endif // !_MY_TEXT_H_
