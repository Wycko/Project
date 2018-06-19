#include "System.h"


System::System()
{
}


System::~System()
{
}

// Initialize
bool System::Init( const HINSTANCE hInstance, const int nCmdShow )
{
	m_hInstance = hInstance;

	if( !Init_Window( nCmdShow ) )
	{
		MessageBox( NULL, L"Failed to create window.", L"Error", MB_OK );
		return false;
	}

	if( !Init_D3D11() )
	{
		MessageBox( m_hWnd, L"Failed to create D3D11-Device.", L"Error", MB_OK );
		return false;
	}

	////////////////////////
	if( !shader.Create( m_Direct3D11->GetDevice(), m_hWnd ) )
	{
		MessageBox( m_hWnd, L"Failed to create shader-object.\nShader file missing?", L"Error", MB_OK );
		return false;
	}

	if( !model.Create( m_Direct3D11->GetDevice() ) )
	{
		MessageBox( m_hWnd, L"Failed to create model-object.", L"Error", MB_OK );
		return false;
	}

	if( !m_Camera.Init( ( float )m_WndWidth, ( float )m_WndHeight ) )
	{
		MessageBox( m_hWnd, L"Failed to create camera-object", L"Error", MB_OK );
		return false;
	}

	m_Camera.Update();

	if( !m_Input.Initialize( m_hWnd ) )
	{
		MessageBox( m_hWnd, L"Failed to create input-object.", L"Error", MB_OK );
		return false;
	}

	// Create the text object.
	m_Text = new TextClass;
	if( !m_Text )
	{
		return false;
	}
	bool result;
	// Initialize the text object.
	result = m_Text->Initialize( m_Direct3D11->GetDevice(), m_Direct3D11->GetDeviceContext(), m_hWnd, m_WndWidth, m_WndHeight, m_Camera.GetViewMatrix() );
	if( !result )
	{
		MessageBox( m_hWnd, L"Could not initialize the text object.", L"Error", MB_OK );
		return false;
	}

	if( !m_MyText.Initialize( m_Direct3D11->GetDevice(), m_Camera.GetViewMatrix() ) )
		return false;

	return true;
}


void System::Run()
{
	MSG msg = { 0 };
	m_GlobalTime.Start();

	bool run = true;
	while( run )
	{
		while( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );

			if( msg.message == WM_QUIT )
				run = false;
		}

		double deltaTime = m_GlobalTime.GetElapsedTime();

		Render( deltaTime );
	}
}

// Clean up
void System::Shutdown()
{
	// Release the text object.
	if( m_Text )
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = nullptr;
	}

	if( m_Direct3D11 )
	{
		m_Direct3D11->Shutdown();
	}

	UnregisterClass( m_WndClassName, m_hInstance );
}


LRESULT WINAPI System::_HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if( msg == WM_NCCREATE )
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast< CREATESTRUCTW* >( lParam );
		System* const pWnd = reinterpret_cast< System* >( pCreate->lpCreateParams );
		// sanity check
		assert( pWnd != nullptr );
		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( pWnd ) );
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( &System::_HandleMsgThunk ) );
		// forward message to window class handler
		return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc( hWnd, msg, wParam, lParam );
}


LRESULT WINAPI System::_HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// retrieve ptr to window class
	System* const pWnd = reinterpret_cast< System* >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
	// forward message to window class handler
	return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
}


LRESULT System::HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

	case WM_CLOSE:
		PostQuitMessage( 0 );
		break;

	case WM_INPUT:
		return m_Input.InputUpdate( lParam );
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


bool System::Init_Window( const int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = _HandleMsgSetup;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW - 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = m_WndClassName;
	wcex.hIconSm = 0;

	if( !RegisterClassEx( &wcex ) )
		return false;

	// Create window
	RECT rc = { 0, 0, m_WndWidth, m_WndHeight };
	AdjustWindowRect( &rc, WS_CAPTION, FALSE );

	m_hWnd = CreateWindow( m_WndClassName, m_WndCaption, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
						   CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_hInstance, this );
	if( !m_hWnd )
		return false;

	ShowWindow( m_hWnd, nCmdShow );
	SetForegroundWindow( m_hWnd );

	return true;
}


bool System::Init_D3D11()
{
	m_Direct3D11 = std::make_unique< Direct3D11 >();
	if( !m_Direct3D11 )
		return false;

	return m_Direct3D11->Init( m_hWnd, m_WndWidth, m_WndHeight );
}


void System::Render( double dt )
{
	HandleInput();

	Update( dt );

	m_Direct3D11->BeginSceen();
	Draw();
	m_Direct3D11->EndSceen();
}


void System::Draw()
{
	model.Render( m_Direct3D11->GetDeviceContext() );
	shader.Render( m_Direct3D11->GetDeviceContext(), m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix() );

	DirectX::XMMATRIX m = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMStoreFloat4x4( &worldMatrix, m );

	m_Logger1.StartLog();
	m_MyText.AddText( buffer1, DirectX::XMFLOAT2( 100.0f, 450.0f ) );
	m_MyText.AddText( buffer2, DirectX::XMFLOAT2( 140.0f, 450.0f ) );
	m_MyText.AddText( buffer3, DirectX::XMFLOAT2( 180.0f, 450.0f ) );
	m_MyText.AddText( "Hello", DirectX::XMFLOAT2( 300.0f, 100.0f ) );
	/*m_MyText.AddText( "Hello World Hello World Hello World Hello World Hello World Hello World\n"
		"Hello World Hello World Hello World Hello World Hello World Hello World\n"
		"Hello World Hello World Hello World Hello World Hello World Hello World\n"
		"Hello World Hello World Hello World Hello World Hello World Hello World\n", DirectX::XMFLOAT2( 400.0f, 300.0f ) );*/
	m_Logger1.StopLog();

	m_Logger2.StartLog();
	m_MyText.RenderText( m_Direct3D11->GetDeviceContext(), m_Camera.GetOrthograficMatrix() );
	m_Logger2.StopLog();

	m_Text->Render( m_Direct3D11->GetDeviceContext(), worldMatrix, m_Camera.GetOrthograficMatrix(), const_cast< char* >( s.c_str() ) );
}


void System::Update( double dt )
{
	m_Timer.Start();

	m_Camera.Update();
	//s = std::to_string( dt );
	framecount++;
	if( m_Timer.GetTime() >= 1000000 )
	{
		double min, max, avg_;
		avg_ = m_Logger1.GetAverage();
		m_Logger1.GetMinMax( min, max );
		m_Logger1.ResetLog();

		m_Timer.Reset();
		//s = std::to_string( framecount ) + "\n" + std::to_string( min2 ) + " | " + std::to_string( max2 ) + " | " + std::to_string( avg2 )/* + "\n"
		//	+ "Hello World Hello World Hello World Hello World Hello World Hello World\n"
		//"Hello World Hello World Hello World Hello World Hello World Hello World\n"
		//"Hello World Hello World Hello World Hello World Hello World Hello World\n"
		//"Hello World Hello World Hello World Hello World Hello World Hello World\n"
		//"Hello World Hello World Hello World Hello World Hello World Hello World\n"
		//"Hello World Hello World Hello World Hello World Hello World Hello World\n"
		//	"Hello World Hello World Hello World Hello World Hello World Hello World\n"*/;
		DirectX::XMFLOAT3 cam = m_Camera.GetRotation();
		s = std::to_string( cam.x ) + " | " + std::to_string( cam.y ) + " | " + std::to_string( cam.z ) + "\n" + std::to_string( dt ) + " | " + std::to_string( max ) + " | " + std::to_string( avg_ );

		avg_ = m_Logger2.GetAverage();
		m_Logger2.GetMinMax( min, max );
		m_Logger2.ResetLog();
		_itoa_s( min, buffer1, 10 );
		_itoa_s( framecount, buffer2, 10 );
		_itoa_s( avg_, buffer3, 10 );

		framecount = 0;
	}
}


void System::HandleInput()
{
	if( m_Input.KeyIsPressed( m_Input.ExitApp ) )
	{
		PostQuitMessage( 0 );
		return;
	}

	m_Camera.Movement( m_Input );

	if( m_Input.MouseIsPressed( m_Input.Mouse_Middle_Button ) )
		m_Camera.LookAtPoint( { 0.0f, 0.0f, 0.0f } );

	if( m_Input.KeyIsPressed( VK_NUMPAD4 ))
		m_Camera.SetRotation( { 0.0f, 0.0f, 0.0f } );

	if( m_Input.KeyIsPressed( 'Q' ) )
		m_Camera.CamReset();

	// Must only be called once at the end.
	m_Input.ResetMouse();
}
