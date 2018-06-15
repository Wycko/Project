#ifndef _INPUHANDLER_H_
#define _INPUHANDLER_H_

#include "Common.h"
#include <bitset>

#define Key_LeftShift 0x2a
#define Key_RightShift 0x36


class InputHandler
{
	friend class System;
public:
	enum MouseButton
	{
		Mouse_Left_Button,
		Mouse_Right_Button,
		Mouse_Middle_Button,
		Mouse_Extra_1_Button,
		Mouse_Extra_2_Button,

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

	bool KeyIsPressed( const KeyboardButton Key ) const;

	bool MouseIsPressed( const MouseButton Button ) const;

	DirectX::XMFLOAT4 GetMouse() const;

private:
	InputHandler();
	~InputHandler();

	bool Initialize( const HWND hWnd );
	LRESULT InputUpdate( LPARAM lparam );
	// Raw input does not reset mouse.lLastX etc.
	void ResetMouse();

	KeyboardButton MapButtons( USHORT Key );
	
	RAWINPUTDEVICE						m_Rid[ 2 ];	
	std::bitset< NumberOfButtons >		m_KeyStates;
	std::bitset< NumberOfMouseButtons >	m_MouseStates;
	DirectX::XMFLOAT4					m_Mouse;
};


#endif // !_INPUHANDLER_H_
