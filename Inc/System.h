#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "Common.h"

#include "Direct3D11.h"
#include "BasicShader.h"
#include "BasicModel.h"

#include "Camera.h"
#include "InputHandler.h"

#include "Text.h"
#include "Timer.h"
#include "PerformanceLogger.h"

#include "MyText.h"


class System
{
public:
	System();
	~System();

	// Initialize
	bool Init( const HINSTANCE hInstance, const int nCmdShow );
	void Run();
	// Clean up
	void Shutdown();

private:
	static LRESULT WINAPI _HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	static LRESULT WINAPI _HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	LRESULT HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	bool Init_Window( const int nCmdShow );
	bool Init_D3D11();

	void Render( double dt );
	void Update( double dt );
	void Draw();
	void HandleInput();
	const wchar_t* m_WndClassName = L"Framework Window";
	const wchar_t* m_WndCaption = L"Test Window";

	HWND m_hWnd;
	HINSTANCE m_hInstance;

	int m_WndWidth = 800;
	int m_WndHeight = 600;

	std::unique_ptr< Direct3D11 > m_Direct3D11;

	/////////////
	BasicShader shader;
	BasicModel model;

	///////////////
	///////////////
	Camera m_Camera;

	InputHandler m_Input;
	std::string s;

	TextClass* m_Text = nullptr;
	Timer m_GlobalTime;
	Timer m_Timer;

	PerformanceLogger m_Logger1;
	PerformanceLogger m_Logger2;
	int framecount = 0;

	char buffer1[ 15 ];
	char buffer2[ 15 ];
	char buffer3[ 15 ];

	MyText m_MyText;
};


#endif // !_SYSTEM_H_
