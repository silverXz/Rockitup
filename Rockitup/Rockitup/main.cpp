
//#include "OglApp.h"
#include "MyApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MyApp glApp(hInstance);
	
	glApp.InitApp(L"Hell yeah!", 300, 300, 1280, 800);

	glApp.InitScene();

	glApp.Run();

	return 0;

}