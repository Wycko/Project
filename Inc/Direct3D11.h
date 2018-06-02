#ifndef _DIRECT3D11_H_
#define _DIRECT3D11_H_

#include "Common.h"


class Direct3D11
{
public:
	Direct3D11();
	~Direct3D11();

	bool Init( const HWND hWnd, const int ScrWidth, const int ScrHeight );

	void BeginSceen();
	void EndSceen();

	ID3D11Device* GetDevice() const
	{
		return m_device.Get();
	}

	ID3D11DeviceContext* GetDeviceContext() const
	{
		return m_deviceContext.Get();
	}
	
	void GetGraphicCardInfo( int& Memory, WCHAR* Name )
	{
		Memory = m_VideoMemory;
		Name = m_VideoName;
	}
	
	void SetRasterizerState( const bool CcwWinding = false, const D3D11_FILL_MODE FillMode = D3D11_FILL_SOLID );
	void Resize( LPARAM lParam );

	// Clean up
	void Shutdown();

private:

	void	CreateAndSetViewport( const float Height, const float Width );
	bool	CreateRasterizerStates();
	

	Microsoft::WRL::ComPtr<	IDXGISwapChain >			m_swapChain;
	Microsoft::WRL::ComPtr<	ID3D11Device >				m_device;
	Microsoft::WRL::ComPtr<	ID3D11DeviceContext >		m_deviceContext;
	Microsoft::WRL::ComPtr<	ID3D11RenderTargetView >	m_renderTargetView;
	Microsoft::WRL::ComPtr<	ID3D11RasterizerState >		m_rasterState[4]; // 1. CW,Solid 2. CCW,Solid 3. CW,Wireframe 4. CCW, Wireframe
	Microsoft::WRL::ComPtr<	ID3D11BlendState >			m_alphaEnableBlendingState;

	int m_VideoMemory = 0;
	WCHAR* m_VideoName = nullptr;
};


#endif // !_DIRECT3D11_H_
