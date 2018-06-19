#include "InputHandler.h"


InputHandler::InputHandler()
{
}


InputHandler::~InputHandler()
{
}


bool InputHandler::Initialize( const HWND hWnd )
{
	m_Rid[ 0 ].usUsagePage	= 0x01;
	m_Rid[ 0 ].usUsage		= 0x02;
	m_Rid[ 0 ].dwFlags		= 0;// m_RidEV_INPUTSINK;
	m_Rid[ 0 ].hwndTarget	= hWnd;

	m_Rid[ 1 ].usUsagePage	= 0x01;
	m_Rid[ 1 ].usUsage		= 0x06;
	m_Rid[ 1 ].dwFlags		= 0;// m_RidEV_INPUTSINK;
	m_Rid[ 1 ].hwndTarget	= hWnd;

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
			auto type = rdi.dwType;
		}
	}
	free( pRawInputDeviceList );

	return RegisterRawInputDevices( m_Rid, 2, sizeof( m_Rid[ 0 ] ) );
}


bool InputHandler::KeyIsPressed( const UINT Key ) const
{
	if( Key < m_NumTotalKeys )
		return m_TotalKeys[ Key ];

	return false;
}


bool InputHandler::KeyIsPressed( const KeyboardButton Key ) const
{
	assert( Key >= 0 && Key < KeyboardButton::NumberOfButtons );
	return m_KeyStates[ Key ];
}


bool InputHandler::MouseIsPressed( const MouseButton Button ) const
{	
	assert( Button >= 0 && Button < MouseButton::NumberOfMouseButtons );
	return m_MouseStates[ Button ];
}


DirectX::XMFLOAT4 InputHandler::GetMouse() const
{
	return m_Mouse;
}


void InputHandler::ResetMouse()
{
	m_Mouse = { 0.0f, 0.0f, 0.0f, 0.0f };
}


LRESULT InputHandler::InputUpdate( LPARAM lparam )
{
	UINT dwSize;

	GetRawInputData( ( HRAWINPUT )lparam, RID_INPUT, NULL, &dwSize, sizeof( RAWINPUTHEADER ) );

	auto lpb = std::make_unique < BYTE[] >( dwSize );
	if( GetRawInputData( ( HRAWINPUT )lparam, RID_INPUT, lpb.get(), &dwSize, sizeof( RAWINPUTHEADER ) ) != dwSize )
		OutputDebugString( TEXT( "GetRawInputData does not return correct size !\n" ) );

	RAWINPUT* raw = reinterpret_cast< RAWINPUT* >( lpb.get() );
	if( raw->header.dwType == RIM_TYPEMOUSE )
	{
		const RAWMOUSE mouse = raw->data.mouse;

		m_Mouse.x = ( float )mouse.lLastX;
		m_Mouse.y = ( float )mouse.lLastY;

		switch( mouse.usButtonFlags )
		{
			case RI_MOUSE_BUTTON_1_DOWN:
				m_MouseStates[ Mouse_Left_Button ] = true;
				break;

			case RI_MOUSE_BUTTON_2_DOWN:
				m_MouseStates[ Mouse_Right_Button ] = true;
				break;

			case RI_MOUSE_BUTTON_3_DOWN:
				m_MouseStates[ Mouse_Middle_Button ] = true;
				break;

			case RI_MOUSE_BUTTON_4_DOWN:
				m_MouseStates[ Mouse_Extra_1_Button ] = true;
				break;

			case RI_MOUSE_BUTTON_5_DOWN:
				m_MouseStates[ Mouse_Extra_2_Button ] = true;
				break;

			case RI_MOUSE_BUTTON_1_UP:
				m_MouseStates[ Mouse_Left_Button ] = false;
				break;

			case RI_MOUSE_BUTTON_2_UP:
				m_MouseStates[ Mouse_Right_Button ] = false;
				break;

			case RI_MOUSE_BUTTON_3_UP:
				m_MouseStates[ Mouse_Middle_Button ] = false;
				break;

			case RI_MOUSE_BUTTON_4_UP:
				m_MouseStates[ Mouse_Extra_1_Button ] = false;
				break;

			case RI_MOUSE_BUTTON_5_UP:
				m_MouseStates[ Mouse_Extra_2_Button ] = false;
				break;

			case RI_MOUSE_WHEEL:
				m_Mouse.z = ( float )( ( short )mouse.usButtonData / WHEEL_DELTA );
				break;

			case RI_MOUSE_HWHEEL:
				m_Mouse.w = ( float )( ( short )mouse.usButtonData / WHEEL_DELTA );
				break;
		}
	}

	else if( raw->header.dwType == RIM_TYPEKEYBOARD )
	{
		const RAWKEYBOARD keyboard = raw->data.keyboard;
		
		USHORT VKeyInfo = 0;
		USHORT VKey		= keyboard.VKey;
		if( VKey == VK_CONTROL || VKey == VK_SHIFT || VKey == VK_MENU )
		{
			switch( VKey )
			{
			case VK_CONTROL:
				VKeyInfo = VK_CONTROL;
				if( keyboard.Flags == RI_KEY_E0 || keyboard.Flags == ( RI_KEY_E0 | RI_KEY_BREAK ) )
					VKey = VK_RCONTROL;

				else
					VKey = VK_LCONTROL;

				break;

			case VK_SHIFT:
				VKeyInfo = VK_SHIFT;
				if( keyboard.MakeCode == Key_LeftShift )
					VKey = VK_LSHIFT;

				else if( keyboard.MakeCode == Key_RightShift )
					VKey = VK_RSHIFT;

				break;

			case VK_MENU:
				VKeyInfo = VK_MENU;
				if( keyboard.Flags == RI_KEY_E0 || keyboard.Flags == ( RI_KEY_E0 | RI_KEY_BREAK ) )
					VKey = VK_RMENU;

				else
					VKey = VK_LMENU;
				break;
			}
		}
		
		const KeyboardButton key = MapButtons( VKey );		
		if( key != Invalid )
		{
			if( keyboard.Flags == RI_KEY_MAKE || keyboard.Flags == RI_KEY_E0 )
				m_KeyStates[ key ] = true;

			else if( keyboard.Flags == RI_KEY_BREAK || keyboard.Flags == ( RI_KEY_E0 | RI_KEY_BREAK ) )
				m_KeyStates[ key ] = false;
		}

		if( keyboard.Flags == RI_KEY_MAKE || keyboard.Flags == RI_KEY_E0 )
		{
			m_TotalKeys[ VKey ] = true;

			if( VKeyInfo != 0 )
				m_TotalKeys[ VKeyInfo ] = true;
		}

		else if( keyboard.Flags == RI_KEY_BREAK || keyboard.Flags == ( RI_KEY_E0 | RI_KEY_BREAK ) )
		{
			m_TotalKeys[ VKey ] = false;

			if( VKeyInfo != 0 )
				m_TotalKeys[ VKeyInfo ] = false;
		}
	}

	else
		return DefRawInputProc( &raw, 1, sizeof( RAWINPUTHEADER ) );

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

	default:
		return KeyboardButton::Invalid;
	}
}
