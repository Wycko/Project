#include "System.h"

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	auto App = std::make_unique< System >();

	if( App )
	{
		if( App->Init( hInstance, nCmdShow ) )
		{
			App->Run();
		}
		App->Shutdown();
	}

	return 0;
}