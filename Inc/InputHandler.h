#ifndef _INPUHANDLER_H_
#define _INPUHANDLER_H_

#include "Common.h"

#include <bitset>
#include <string>

#define Key_LeftShift 0x2a
#define Key_RightShift 0x36


class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	enum MouseButton
	{
		Mouse_Left_Button,
		Mouse_Right_Button,
		Mouse_Middle_Button,

		NumberOfMouseButtons
	};

	enum KeyboardButton
	{
		// Camera movements.
		Movement_Forward,
		Movement_Backward,
		Movement_Left,
		Movement_Right,
		Movement_Up,
		Movement_Down,
		// System key events.
		ExitApp,

		NumberOfButtons,
		Invalid = -1
	};

	bool Initialize( const HWND hWnd );

	bool KeyIsPressed( const KeyboardButton Key ) const;
	bool MouseIsPressed( const MouseButton Button ) const;

	DirectX::XMFLOAT3 GetMouseMove();
	void GetWheelData( short& roll, short& side )
	{
		if( roll != 0 )
			roll = m_Mouse.m_lastWheelY / 12;

		if( side != 0 )
			side = m_Mouse.m_lastWheelX / 12;
	}

	void OnKeyPressed( const char Key );
	void OnKeyReleased( const char Key );

	LRESULT InputUpdate( LPARAM lparam );

private:
	class Mouse
	{
	public:
		Mouse();
		~Mouse();

		void Process( RAWMOUSE mouse );

		std::bitset< NumberOfMouseButtons > MouseStates;
		DirectX::XMFLOAT2 m_MouseLast;
		// Scroll up and down
		short m_lastWheelY = 0;
		// Wheel left and right click
		short m_lastWheelX = 0;
	};
	KeyboardButton MapButtons( USHORT Key );
	
	RAWINPUTDEVICE Rid[ 2 ];
	std::string s = "test";
	
	std::bitset< NumberOfButtons > KeyStates;

	Mouse m_Mouse;
};


#endif // !_INPUHANDLER_H_
