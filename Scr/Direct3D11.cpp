#include "Direct3D11.h"


Direct3D11::Direct3D11()
{
}


Direct3D11::~Direct3D11()
{
}


bool Direct3D11::Init( const HWND hWnd, const int ScrWidth, const int ScrHeight )
{
	HRESULT hr;
	IDXGIFactory* factory = nullptr;
	DXGI_MODE_DESC e;
	// Get video card info
	{
		//IDXGIFactory* factory = nullptr;
		IDXGIAdapter* adapter = nullptr;
		IDXGIOutput* adapterOutput;
		DXGI_ADAPTER_DESC adapterDesc;
		DXGI_OUTPUT_DESC desc;

		// Create a DirectX graphics interface factory.
		hr = CreateDXGIFactory( __uuidof(IDXGIFactory), (void**)&factory );
		if( FAILED( hr ) )
			return false;

		// Use the factory to create an adapter for the primary graphics interface (video card).
		hr = factory->EnumAdapters( 0, &adapter );
		if( FAILED( hr ) )
		{
			factory->Release();
			factory = nullptr;
			return false;
		}

		adapter->EnumOutputs( 0, &adapterOutput );

		UINT numModes = 0;
		DXGI_MODE_DESC* displayModes = NULL;
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

		DXGI_MODE_DESC d;
		d.Format = format;
		d.Height = ScrHeight;
		d.RefreshRate.Denominator = 0;
		d.RefreshRate.Numerator = 0;
		d.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		d.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		d.Width = ScrWidth;

		hr = adapterOutput->FindClosestMatchingMode( &d, &e, NULL );

		// Get the number of elements		
		hr = adapterOutput->GetDisplayModeList( format, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL );

		displayModes = new DXGI_MODE_DESC[ numModes ];

		// Get the list
		hr = adapterOutput->GetDisplayModeList( format, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModes );

		for( UINT i = 0; i < numModes; ++i )
		{
			DXGI_MODE_DESC w;
			w = displayModes[ i ];
		}

		delete[] displayModes;
		adapterOutput->GetDesc( &desc );
		adapterOutput->Release();


		// Get the adapter (video card) description.
		hr = adapter->GetDesc( &adapterDesc );
		if( FAILED( hr ) )
		{
			adapter->Release();
			adapter = nullptr;
			factory->Release();
			factory = nullptr;

			return false;
		}

		// Release the adapter.
		adapter->Release();
		adapter = nullptr;

		// Release the factory.
		factory->Release();
		factory = nullptr;

		m_VideoMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		m_VideoName = adapterDesc.Description;
	}

	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory( &scd, sizeof( DXGI_SWAP_CHAIN_DESC ) );

	// fill the swap chain description struct
	scd.BufferCount = 1;								// one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// use 32-bit color
	scd.BufferDesc.Width = ScrWidth;					// set the back buffer width
	scd.BufferDesc.Height = ScrHeight;					// set the back buffer height
	scd.BufferDesc = e;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// how swap chain is to be used
	scd.OutputWindow = hWnd;							// the window to be used
	scd.SampleDesc.Count = 4;							// how many multisamples
	scd.Windowed = TRUE;								// windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// allow full-screen switching

	D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_9_1;
	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// create a device, device context and swap chain using the information in the scd struct
	hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, FeatureLevels, ARRAYSIZE( FeatureLevels ),
										D3D11_SDK_VERSION, &scd, m_swapChain.GetAddressOf(), m_device.GetAddressOf(), &MaxSupportedFeatureLevel, m_deviceContext.GetAddressOf() );
	if( FAILED( hr ) )
		return false;

	hr = m_swapChain->GetParent( __uuidof( IDXGIFactory ), ( LPVOID* )&factory );

	hr = factory->MakeWindowAssociation( hWnd, DXGI_MWA_NO_WINDOW_CHANGES );

	factory->Release();

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer = nullptr;
	hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
		return false;

	// use the back buffer address to create the render target
	hr = m_device->CreateRenderTargetView( pBackBuffer, NULL, m_renderTargetView.GetAddressOf() );
	if( FAILED( hr ) )
	{
		pBackBuffer->Release();
		pBackBuffer = nullptr;
		return false;
	}

	pBackBuffer->Release();
	pBackBuffer = nullptr;

	// set the render target as the back buffer
	m_deviceContext->OMSetRenderTargets( 1, m_renderTargetView.GetAddressOf(), NULL );

	// Create the rasterizer state from the description.	
	hr = CreateRasterizerStates();
	if( FAILED( hr ) )
		return false;
	
	// Now set the rasterizer state.
	SetRasterizerState();

	// Set the viewport
	CreateAndSetViewport( ( float )ScrHeight, ( float )ScrWidth );

	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory( &blendStateDescription, sizeof( D3D11_BLEND_DESC ) );

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = m_device->CreateBlendState( &blendStateDescription, m_alphaEnableBlendingState.GetAddressOf() );
	if( FAILED( hr ) )
		return false;

	float blendFactor[4];
	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState( m_alphaEnableBlendingState.Get(), blendFactor, 0xffffffff );
	return true;
}


void Direct3D11::BeginSceen()
{
	// clear the back buffer to a deep blue
	float c[4] = { 0.0f, 0.7f, 0.4f, 1.0f };

	m_deviceContext->ClearRenderTargetView( m_renderTargetView.Get(), DirectX::Colors::AliceBlue );
}


void Direct3D11::EndSceen()
{
	m_swapChain->Present( 0, 0 );
}


// Clean up
void Direct3D11::Shutdown()
{	
	// close and release all existing COM objects
	if( m_swapChain )
	{
		// switch to windowed mode
		m_swapChain->SetFullscreenState( FALSE, NULL );
	}
}


void Direct3D11::CreateAndSetViewport( const float Height, const float Width )
{
	D3D11_VIEWPORT viewport;
	ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = ScrHeight;

	m_deviceContext->RSSetViewports( 1, &viewport );
}


void Direct3D11::SetRasterizerState( const bool CcwWinding = false, const D3D11_FILL_MODE FillMode = D3D11_FILL_SOLID )
{
	switch( FillMode )
	{
		case D3D11_FILL_SOLID:
			if( !CcwWinding )
				m_deviceContext->RSSetState( m_rasterState[0].Get() );
			
			else
				m_deviceContext->RSSetState( m_rasterState[1].Get() );
			
			break;
			
		case D3D11_FILL_WIREFRAME:
			if( !CcwWinding )
				m_deviceContext->RSSetState( m_rasterState[2].Get() );
			
			else
				m_deviceContext->RSSetState( m_rasterState[3].Get() );
			
			break;
	}
}


HRESULT Direct3D11::CreateRasterizerStates()
{
	HRESULT hr = S_OK;
	D3D11_RASTERIZER_DESC rasterDesc;
	
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = m_device->CreateRasterizerState( &rasterDesc, m_rasterState[0].GetAddressOf() );
	if( FAILED( hr ) )
		return hr;	
	
	rasterDesc.FrontCounterClockwise = true;	
	
	hr = m_device->CreateRasterizerState( &rasterDesc, m_rasterState[1].GetAddressOf() );
	if( FAILED( hr ) )
		return hr;
	
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	
	hr = m_device->CreateRasterizerState( &rasterDesc, m_rasterState[3].GetAddressOf() );
	if( FAILED( hr ) )
		return hr;
	
	rasterDesc.FrontCounterClockwise = false;	
	
	hr = m_device->CreateRasterizerState( &rasterDesc, m_rasterState[2].GetAddressOf() );
	if( FAILED( hr ) )
		return hr;	
}


void Direct3D11::Resize( LPARAM lParam )
{
	UINT width, height;
	
	height = HIWORD( lParam );
	width = LOWORD( lParam );
	
	m_deviceContext->ClearState();
	m_swapChain->ResizeBuffers( 1, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
	
	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer = nullptr;
	hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
		return false;

	// use the back buffer address to create the render target
	hr = m_device->CreateRenderTargetView( pBackBuffer, NULL, m_renderTargetView.GetAddressOf() );
	if( FAILED( hr ) )
	{
		pBackBuffer->Release();
		pBackBuffer = nullptr;
		return false;
	}

	pBackBuffer->Release();
	pBackBuffer = nullptr;

	// set the render target as the back buffer
	m_deviceContext->OMSetRenderTargets( 1, m_renderTargetView.GetAddressOf(), NULL );
	
	CreateAndSetViewport( ( float )height, ( float )width );
	SetRasterizerState();	
}