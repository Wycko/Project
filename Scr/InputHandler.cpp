#include "InputHandler.h"


InputHandler::InputHandler()
{
}


InputHandler::~InputHandler()
{
}


bool InputHandler::Initialize( const HWND hWnd )
{
	Rid[ 0 ].usUsagePage = 0x01;
	Rid[ 0 ].usUsage = 0x02;
	Rid[ 0 ].dwFlags = 0;// RIDEV_INPUTSINK;
	Rid[ 0 ].hwndTarget = hWnd;

	Rid[ 1 ].usUsagePage = 0x01;
	Rid[ 1 ].usUsage = 0x06;
	Rid[ 1 ].dwFlags = 0;// RIDEV_INPUTSINK;
	Rid[ 1 ].hwndTarget = hWnd;

	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;

	if( GetRawInputDeviceList( NULL, &nDevices, sizeof( RAWINPUTDEVICELIST ) ) != 0 )
		return false;

	if( ( pRawInputDeviceList = reinterpret_cast< PRAWINPUTDEVICELIST >( malloc( sizeof( RAWINPUTDEVICELIST ) * nDevices ) ) ) == NULL )
		return false;

	if( GetRawInputDeviceList( pRawInputDeviceList, &nDevices, sizeof( RAWINPUTDEVICELIST ) ) != -1 )
	{
		for( int i = 0; i < ( int )nDevices; i++ )
		{
			WCHAR data[ 256 ] = { 0 };
			unsigned int num = 256;
			GetRawInputDeviceInfo( pRawInputDeviceList[ i ].hDevice, RIDI_DEVICENAME, data, &num );

			RID_DEVICE_INFO rdi;
			rdi.cbSize = sizeof( RID_DEVICE_INFO );

			UINT cbSize = rdi.cbSize;
			GetRawInputDeviceInfo( pRawInputDeviceList[ i ].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize );
		}
	}

	free( pRawInputDeviceList );

	return RegisterRawInputDevices( Rid, 2, sizeof( Rid[ 0 ] ) );
}

bool InputHandler::KeyIsPressed( const KeyboardButton Key ) const
{
	return KeyStates[ Key ];
}

bool InputHandler::MouseIsPressed( const MouseButton Button ) const
{
	return m_Mouse.MouseStates[ Button ];
}

DirectX::XMFLOAT3 InputHandler::GetMouseMove() const
{
	return DirectX::XMFLOAT3( m_Mouse.m_MouseLast.y, m_Mouse.m_MouseLast.x, 0.0f );
}

void InputHandler::OnKeyPressed( const char Key )
{
	KeyboardButton key = MapButtons( Key );
	if( key >= 0 && key < NumberOfButtons )
	{
		KeyStates[ key ] = true;
	}
}

void InputHandler::OnKeyReleased( const char Key )
{
	KeyboardButton key = MapButtons( Key );
	if( key >= 0 && key < NumberOfButtons )
	{
		KeyStates[ key ] = false;
	}
}

LRESULT InputHandler::InputUpdate( LPARAM lparam )
{
	UINT dwSize;

	GetRawInputData( ( HRAWINPUT )lparam, RID_INPUT, NULL, &dwSize, sizeof( RAWINPUTHEADER ) );
	
	auto lpb = std::make_unique< new BYTE[ dwSize ] >();
	//LPBYTE lpb = new BYTE[ dwSize ];

	if( GetRawInputData( ( HRAWINPUT )lparam, RID_INPUT, lpb.get(), &dwSize, sizeof( RAWINPUTHEADER ) ) != dwSize )
		OutputDebugString( TEXT( "GetRawInputData does not return correct size !\n" ) );

	RAWINPUT* raw = reinterpret_cast< RAWINPUT* >( lpb.get() );	

	if( raw->header.dwType == RIM_TYPEMOUSE )
	{
		m_Mouse.Process( raw->data.mouse );		
		s = "x = " + std::to_string( ( int )m_Mouse.m_MouseLast.x ) + " | y = " + std::to_string( ( int )m_Mouse.m_MouseLast.y );
	}
	
	else if( raw->header.dwType == RIM_TYPEKEYBOARD )
	{
		const RAWKEYBOARD keyboard = raw->data.keyboard;		
		USHORT VKey = keyboard.VKey;

		if( VKey == VK_CONTROL || VK_SHIFT || VK_MENU )
		{
			switch( VKey )
			{
			case VK_CONTROL:
				if( keyboard.Flags == RI_KEY_E0 || keyboard.Flags == ( RI_KEY_E0 | RI_KEY_BREAK ) )
					VKey = VK_RCONTROL;

				else
					VKey = VK_LCONTROL;

				break;

			case VK_SHIFT:
				if( keyboard.MakeCode == Key_LeftShift )
					VKey = VK_LSHIFT;

				else if( keyboard.MakeCode == Key_RightShift )
					VKey = VK_RSHIFT;

				break;

			case VK_MENU:
				if( keyboard.Flags == RI_KEY_E0 || keyboard.Flags == ( RI_KEY_E0 | RI_KEY_BREAK ) )
					VKey = VK_RMENU;

				else
					VKey = VK_LMENU;
				break;
			}
		}

		cosnt KeyboardButton key = MapButtons( VKey );
		
		if( key != Invalid && key < NumberOfButtons )
		{
			if( keyboard.Flags == RI_KEY_MAKE || keyboard.Flags == RI_KEY_E0 )
			{
				KeyStates[ key ] = true;
			}
			else if( keyboard.Flags == RI_KEY_BREAK || keyboard.Flags == ( RI_KEY_E0 | RI_KEY_BREAK ) )
			{
				KeyStates[ key ] = false;
			}
		}
	}
	
	else
		return DefRawInputProc( raw, 1, sizeof( RAWINPUTHEADER ) );

	//delete[] lpb;

	return 0;
}

InputHandler::KeyboardButton InputHandler::MapButtons( USHORT Key )
{
	switch( Key )
	{
	case 'W':
		return KeyboardButton::Movement_Forward;

	case 'S':
		return KeyboardButton::Movement_Backward;

	case 'D':
		return KeyboardButton::Movement_Right;

	case 'A':
		return KeyboardButton::Movement_Left;

	case VK_SPACE:
		return KeyboardButton::Movement_Up;

	case VK_LCONTROL:
		return KeyboardButton::Movement_Down;

	case VK_RCONTROL:
		return KeyboardButton::Movement_Up;

	case VK_RSHIFT:
		return KeyboardButton::Movement_Right;

	case VK_LSHIFT:
		return KeyboardButton::Movement_Left;

	case VK_RMENU:
		return KeyboardButton::Movement_Backward;

	case VK_LMENU:
		return KeyboardButton::Movement_Forward;

	case VK_ESCAPE:
		return KeyboardButton::ExitApp;
	}

	return KeyboardButton::Invalid;
}

InputHandler::Mouse::Mouse()
{
}

InputHandler::Mouse::~Mouse()
{
}

void InputHandler::Mouse::Process( RAWMOUSE mouse )
{
	m_MouseLast.x += ( float )mouse.lLastX;
	m_MouseLast.y += ( float )mouse.lLastY;

	switch( mouse.usButtonFlags )
	{
	case RI_MOUSE_BUTTON_1_DOWN:
		MouseStates[ Mouse_Left_Button ] = true;
		break;

	case RI_MOUSE_BUTTON_2_DOWN:
		MouseStates[ Mouse_Right_Button ] = true;
		break;

	case RI_MOUSE_BUTTON_3_DOWN:
		MouseStates[ Mouse_Middle_Button ] = true;
		break;

	case RI_MOUSE_BUTTON_1_UP:
		MouseStates[ Mouse_Left_Button ] = false;
		break;

	case RI_MOUSE_BUTTON_2_UP:
		MouseStates[ Mouse_Right_Button ] = false;
		break;

	case RI_MOUSE_BUTTON_3_UP:
		MouseStates[ Mouse_Middle_Button ] = false;
		break;

	case RI_MOUSE_WHEEL:
		m_lastWheelY += ( short )mouse.usButtonData;
		break;
		
	case RI_MOUSE_HWHEEL:
		m_lastWheelX += ( short )mouse.usButtonData;
		break;
	}
}
