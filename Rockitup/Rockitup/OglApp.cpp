#include "OglApp.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>
// For the win32 console
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <commdlg.h>
#include <time.h>

#include <gl/glew.h>
#include <gl/wglew.h>

OglApp::OglApp(HINSTANCE hInstance,BOOL bShowConsole) : mhInstance(hInstance),mbRunning(FALSE),mbFullScreen(FALSE),mbMaximized(FALSE),mbMessageBumpping(FALSE),mbMultisample(FALSE),
mbWglSupportMultisample(FALSE), mbVisible(FALSE)
{
	mArbMultisamplePixelFormat = 0;
	mhWnd = NULL;
	mhDC = NULL;
	mhGlhc = NULL;
	mUpdateTimeGap = 0.0;
	mMinUpdateTimeGap = 4.0;
	if (bShowConsole)
		ShowWin32Console();
}


OglApp::~OglApp()
{

}

void OglApp::ShowWin32Console()
{
#if defined(_WIN32)
	AllocConsole();
	// stdout
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;
	// stderr
	handle_out = GetStdHandle(STD_ERROR_HANDLE);
	hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
	hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stderr = *hf_out;
#endif
}

void OglApp::Terminate()
{
	PostMessage(mhWnd, WM_QUIT, 0, 0);
	mbRunning = FALSE;
}

void OglApp::ToggleFullScreen()
{
	PostMessage(mhWnd, WM_TOGGLEFULLSCREEN, 0, 0);
}

BOOL OglApp::ChangeScreenResolution(int width, int height, int bitsPerPixel)
{
	DEVMODE dmScreenSettings;  //Device Mode
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));		//Make Sure Memory Is Cleared
	dmScreenSettings.dmSize = sizeof(DEVMODE);
	dmScreenSettings.dmPelsWidth = width;
	dmScreenSettings.dmPelsHeight = height;
	dmScreenSettings.dmBitsPerPel = bitsPerPixel;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		return FALSE;
	return TRUE;
}


void OglApp::InitApp(wchar_t* title, int x, int y, int width, int height, int bits/*32*/, bool fullsrceen /*= false*/, bool maximized /*= false*/, bool multisampled /*= false*/, bool bVSync)
{
	if (!_InitApp(title, x, y, width, height, bits, fullsrceen, maximized, multisampled,bVSync))
	{
		std::cout << "Failed to initialize application!\n";
		DestroyApp();
	}
	else
		mbRunning = TRUE;
}


BOOL OglApp::_InitApp(wchar_t* title, int x, int y, int width, int height, int bits, bool fullsrceen, bool maximized, bool multisampled, bool bVSync)
{
	mTitle = title;
	mPosX = x;
	mPosY = y;
	mWidth = width;
	mHeight = height;
	mBitsPerPixel = bits;
	mbFullScreen = fullsrceen;
	mbMultisample = multisampled;
	mbMaximized = maximized;

	if (RegisterWindowClass() == FALSE)
	{
		printf("Error Registering Window Class!\n");
		return FALSE;
	}

	printf("Class Registration Done.....\n");

	if (InitOgl() == FALSE)
	{
		UnregisterClass(L"OpenGL", mhInstance);
		return FALSE;
	}

	printf("OpenGL Window Creation Done.....\n");

	if (bVSync)
		SetVSync(bVSync);

	mbRunning = TRUE;
	mLastTime = clock();
	return TRUE;
}

BOOL OglApp::RegisterWindowClass()
{
	//Register A window Class
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = (WNDPROC)(WindowProc);
	windowClass.hInstance = mhInstance;
	windowClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"OpenGL";

	if (RegisterClassEx(&windowClass) == 0)
	{
		MessageBox(HWND_DESKTOP, L"RegisterClassEx Failed!", L"Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL OglApp::InitOgl()
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;

	PIXELFORMATDESCRIPTOR pfd =								//pfd Tells windows How We want things to be
	{
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		mBitsPerPixel,
		0, 0, 0, 0, 0, 0,
		1,							//Alpha buffer
		0,							//Shift buffer ignored
		0,							//No Accumulation buffer
		0, 0, 0, 0,					//Accumulation bufer ignored
		16,						//16 bits Z-buffer (depth buffer)
		0,
		0,
		PFD_MAIN_PLANE,	//Main Drawing Layer
		0,
		0, 0, 0
	};

	RECT windowRect = { 0, 0, mWidth, mHeight};

	GLuint PixelFormat;

	if (mbFullScreen == TRUE)								// Fullscreen Requested, Try Changing Video Modes
	{
		if (ChangeScreenResolution(mWidth, mHeight, mBitsPerPixel) == FALSE)
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox(HWND_DESKTOP, L"Mode Switch Failed.\nRunning In Windowed Mode.", L"Error", MB_OK | MB_ICONEXCLAMATION);
			mbFullScreen = FALSE;							// Set isFullscreen To False (Windowed Mode)
		}
		else															// Otherwise (If Fullscreen Mode Was Successful)
		{
			ShowCursor(FALSE);											// Turn Off The Cursor
			windowStyle = WS_POPUP;										// Set The WindowStyle To WS_POPUP (Popup Window)
			windowExtendedStyle |= WS_EX_TOPMOST;						// Set The Extended Window Style To WS_EX_TOPMOST
		}																// (Top Window Covering Everything Else)
	}
	else																// If Fullscreen Was Not Selected
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// Create The OpenGL Window
	//HMENU hMenu = LoadMenu(mhInstance, MAKEINTRESOURCE(IDR_MENU1));

	mhWnd = CreateWindowEx(windowExtendedStyle,					// Extended Style
		L"OpenGL",	// Class Name
		mTitle.c_str(),
		//L"OglApp",					// Window Title
		windowStyle,							// Window Style
		mPosX, mPosY,								// Window X,Y Position
		windowRect.right - windowRect.left,	// Window Width
		windowRect.bottom - windowRect.top,	// Window Height
		HWND_DESKTOP,						// Desktop Is Window's Parent
		0,
		mhInstance, // Pass The Window Instance
		this);

	if (mhWnd == 0)												// Was Window Creation A Success?
	{
		return FALSE;													// If Not Return False
	}

	mhDC = GetDC(mhWnd);									// Grab A Device Context For This Window
	if (mhDC == 0)												// Did We Get A Device Context?
	{
		// Failed
		DestroyWindow(mhWnd);									// Destroy The Window
		mhWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}


	/*
	Our first pass, Multisampling hasn't been created yet, so we create a window normally
	If it is supported, then we're on our second pass
	that means we want to use our pixel format for sampling
	so set PixelFormat to arbMultiSampleformat instead
	*/


	if (!mbWglSupportMultisample)
	{
		PixelFormat = ChoosePixelFormat(mhDC, &pfd);				// Find A Compatible Pixel Format
		if (PixelFormat == 0)												// Did We Find A Compatible Format?
		{
			// Failed
			ReleaseDC(mhWnd, mhDC);							// Release Our Device Context
			mhDC = 0;												// Zero The Device Context
			DestroyWindow(mhWnd);									// Destroy The Window
			mhWnd = 0;												// Zero The Window Handle
			return FALSE;													// Return False
		}

	}
	else
	{
		PixelFormat = mArbMultisamplePixelFormat;
	}

	if (SetPixelFormat(mhDC, PixelFormat, &pfd) == FALSE)		// Try To Set The Pixel Format
	{
		// Failed
		ReleaseDC(mhWnd, mhDC);							// Release Our Device Context
		mhDC = 0;												// Zero The Device Context
		DestroyWindow(mhWnd);									// Destroy The Window
		mhWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	mhGlhc = wglCreateContext(mhDC);						// Try To Get A Rendering Context
	if (mhGlhc == 0)												// Did We Get A Rendering Context?
	{
		// Failed
		ReleaseDC(mhWnd, mhDC);							// Release Our Device Context
		mhDC = 0;												// Zero The Device Context
		DestroyWindow(mhWnd);									// Destroy The Window
		mhWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	if (wglMakeCurrent(mhDC, mhGlhc) == FALSE)
	{
		// Failed
		wglDeleteContext(mhGlhc);									// Delete The Rendering Context
		mhGlhc = 0;												// Zero The Rendering Context
		ReleaseDC(mhWnd, mhDC);							// Release Our Device Context
		mhDC = 0;												// Zero The Device Context
		DestroyWindow(mhWnd);									// Destroy The Window
		mhWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		// Failed
		wglDeleteContext(mhGlhc);									// Delete The Rendering Context
		mhGlhc = 0;												// Zero The Rendering Context
		ReleaseDC(mhWnd, mhDC);							// Release Our Device Context
		mhDC = 0;												// Zero The Device Context
		DestroyWindow(mhWnd);									// Destroy The Window
		mhWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}


	if (!mbWglSupportMultisample && CHECK_FOR_MULTISAMPLE)
	{

		if (InitMultisample(mhInstance, mhWnd, pfd))
		{
			DestroyApp();
			return InitOgl();
		}
	}

	if (mbMaximized)
		ShowWindow(mhWnd, SW_MAXIMIZE);								// Make The Window Visible
	else
		ShowWindow(mhWnd, SW_NORMAL);
	mbVisible = TRUE;											// Set isVisible To True

	int glVersion[2] = { -1, -1 };
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
	printf("Current OpenGL Version: %d.%d\n", glVersion[0], glVersion[1]);


	Resize(mWidth, mHeight);				// Reshape Our GL Window

	ZeroMemory(&mKeys, sizeof(Keys));							// Clear All Keys

	//m_lastTickCount = GetTickCount();							// Get Tick Count

	return TRUE;														// Window Creating Was A Success
}

BOOL OglApp::InitMultisample(HINSTANCE hInstance, HWND hWnd, PIXELFORMATDESCRIPTOR pfd)
{
	//See If The String Exists In WGL!
	if (!IsWglExtensionSupported("WGL_ARB_multisample"))
	{
		mbWglSupportMultisample = FALSE;
		return FALSE;
	}


	//we can also use glew Function to find out if WGL_ARB_multisample is supported.

	if (wglewIsSupported("WGL_ARB_multisample") != 1)
	{
		mbWglSupportMultisample = false;
		return false;
	}

	//Get Our Pixel Format
	//PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	//if(!wglChoosePixelFormatARB)
	//{
	//	arbMultisampleSupported = false;
	//	return false;
	//}

	//Get Our Current Device Context
	HDC hDc = GetDC(hWnd);

	int pixelFormat;
	int valid;
	UINT numFormats;
	float fAttributes[] = { 0, 0 };

	//These Attributes Are The Bits We Want To Test For In Our Sample
	//Everything Is Pretty Standard, The Only One We Want to 
	//Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
	//These Two Are Going To Do The Main Testing For Whether Or Not 
	//We Support Multisampling On This Hardware.
	int iAttributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 24,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,   //ÆôÓÃÄ£°å»º³åÇø8bit
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,
		0, 0
	};

	//First We Check To See If we can get a pixel format for 4 Samples
	valid = wglChoosePixelFormatARB(hDc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

	//If We Retruned True, And Our Format Count Is Greater Than 1
	if (valid && numFormats >= 1)
	{
		mbWglSupportMultisample = TRUE;
		printf("MultiSample Supported!\n");
		mArbMultisamplePixelFormat = pixelFormat;
		return mbWglSupportMultisample;
	}

	//Our PixelFormat with 4 Samples Failed, Test for 2 Samples
	iAttributes[19] = 2;
	valid = wglChoosePixelFormatARB(hDc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
	if (valid && numFormats >= 1)
	{
		mbWglSupportMultisample = TRUE;
		mArbMultisamplePixelFormat = pixelFormat;
		return mArbMultisamplePixelFormat;
	}

	//Return The Valid Format
	return mbWglSupportMultisample;
}

BOOL OglApp::IsWglExtensionSupported(const char* extension)
{
	const size_t extlen = strlen(extension);
	const char* supported = NULL;

	//Try to Use wglGetExtensionStringARB On Current DC, if Possible.
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

	if (wglGetExtString)
		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());

	//If That Failed, Try Standard OpenGL Extensions String
	if (supported == NULL)
		supported = (char*)glGetString(GL_EXTENSIONS);

	//If That Failed TOO, Must Be No Extensions Supported!
	if (supported == NULL)
		return false;					//There is no match extensions

	//Begin Examination At Start Of String, Increment By 1 On False Match
	for (const char* p = supported;; p++)
	{
		//Advance p Up to The Next Possible Match
		p = strstr(p, extension);     // strstr(char* target,char* source): find the first appearance of target in source string.

		if (p == NULL)
			return false;				//No Match!

		//Make Sure That Match Is At The Start Of The String Or That
		//The Previous Char Is A Space, Or Else We Could Accidentally
		//Match "wglFunkywglExtension" With "wglExtension"

		//Also, Make Sure That The Following Character is Space Or NULL
		//Or Else "wglExtensionTwo" Might Match "wglExtension"

		if ((p == supported || p[-1] == ' ') && (p[extlen] == '\0' || p[extlen] == ' '))
			return true;
	}
}

BOOL OglApp::DestroyApp()
{
	if (mhWnd != 0)												// Does The Window Have A Handle?
	{
		if (mhDC != 0)											// Does The Window Have A Device Context?
		{
			wglMakeCurrent(mhDC, 0);							// Set The Current Active Rendering Context To Zero
			if (mhGlhc != 0)										// Does The Window Have A Rendering Context?
			{
				wglDeleteContext(mhGlhc);							// Release The Rendering Context
				mhGlhc = 0;										// Zero The Rendering Context

			}
			ReleaseDC(mhWnd, mhDC);						// Release The Device Context
			mhDC = 0;											// Zero The Device Context
		}
		DestroyWindow(mhWnd);									// Destroy The Window
		mhWnd = 0;												// Zero The Window Handle
	}

	if (mbFullScreen)										// Is Window In Fullscreen Mode
	{
		ChangeDisplaySettings(NULL, 0);									// Switch Back To Desktop Resolution
		ShowCursor(TRUE);												// Show The Cursor
	}
	return TRUE;
}

LRESULT CALLBACK OglApp::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OglApp* pApp = NULL;

	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pApp = reinterpret_cast<OglApp*>(lpcs->lpCreateParams);
		pApp->mhWnd = hWnd;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pApp));
	}
	else
	{
		pApp = reinterpret_cast<OglApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pApp)
		return pApp->HandleMessage(uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT OglApp::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	//LRESULT lres;
	switch (msg)														// Evaluate Window Message
	{
	case WM_SYSCOMMAND:												// Intercept System Commands
	{
		switch (wParam)												// Check System Calls
		{
		case SC_SCREENSAVE:										// Screensaver Trying To Start?
		case SC_MONITORPOWER:									// Monitor Trying To Enter Powersave?
			return 0;												// Prevent From Happening
		}
		break;														// Exit
	}
	return 0;														// Return

// 	case WM_COMMAND:
// 	{
// 		switch (LOWORD(wParam))
// 		{
// 		case ID_FILE_EXIT:
// 			PostMessage(m_hWnd, WM_QUIT, 0, 0);
// 			break;
// 		case ID_FILE_OPEN40001:
// 		{
// 			OpenFileDialog ofd;
// 			if (ofd.ShowDialog())
// 			{
// 				std::string filePath = ofd.FileName;
// 				CxMesh* pMesh = CxFileIO::ReadFile(filePath.c_str());
// 				pMesh->CreateAdjacency();
// 				pMesh->CalculateNormalPerFace();
// 				pMesh->CalculateNormalPerVertex();
// 				pMesh->CalculateBoundingBox();
// 				m_pScene->SetMesh(pMesh);
// 				pMesh->drop();
// 				printf("Sucessfully opened file...\n");
// 			}
// 			else
// 				printf("Failed to open files!\n");
// 			break;
// 		}
// 		default:
// 			break;
// 		}
// 	}
// 	return 0;


	case WM_CREATE:													// Window Creation
	{
		//int sb = 0.0;
	}
	return 0;														// Return

	case WM_CLOSE:													// Closing The Window
		Terminate();								// Terminate The Application
		return 0;														// Return

	case WM_SIZE:													// Size Action Has Taken Place
		switch (wParam)												// Evaluate Size Action
		{
		case SIZE_MINIMIZED:									// Was Window Minimized?
			mbVisible = FALSE;							// Set isVisible To False
			return 0;												// Return

		case SIZE_MAXIMIZED:									// Was Window Maximized?
			mbVisible = TRUE;							// Set isVisible To True
			Resize(LOWORD(lParam), HIWORD(lParam));		// Reshape Window - LoWord=Width, HiWord=Height
			return 0;												// Return

		case SIZE_RESTORED:										// Was Window Restored?
			mbVisible = TRUE;							// Set isVisible To True
			Resize(LOWORD(lParam), HIWORD(lParam));		// Reshape Window - LoWord=Width, HiWord=Height
			return 0;												// Return
		}
		break;															// Break

	case WM_KEYDOWN:												// Update Keyboard Buffers For Keys Pressed
	{
		if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
		{
			mKeys.keyDown[wParam] = TRUE;					// Set The Selected Key (wParam) To True
			return 0;												// Return
		}
		break;															// Break
	}

	case WM_LBUTTONDOWN:							//Mouse Left button down
	{
// 		m_bLeftButtonDown = TRUE;
// 		int xPos = GET_X_LPARAM(lParam);
// 		int yPos = GET_Y_LPARAM(lParam);
// 		m_pScene->Pinch(xPos, yPos);
// 		m_pScene->GetPinched() = true;
		return 0;
		break;
	}

	case WM_LBUTTONUP:									//Mouse Left button up
	{
// 		m_bRightButtonDown = FALSE;
// 		int xPos = GET_X_LPARAM(lParam);
// 		int yPos = GET_Y_LPARAM(lParam);
// 		m_pScene->Release(xPos, yPos);
// 		m_pScene->GetPinched() = false;
		return 0;
		break;
	}

	case WM_MOUSEMOVE:								//
	{
// 		int xPos = GET_X_LPARAM(lParam);
// 		int yPos = GET_Y_LPARAM(lParam);
// 		m_pScene->Move(xPos, yPos);
		return 0;
	}
	break;

	case WM_MOUSEWHEEL:								//
	{
// 		if (m_pScene == NULL)
// 			return 0;
// 		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
// 		if (zDelta > 0)
// 			m_pScene->ZoomOut();
// 		else if (zDelta < 0)
// 			m_pScene->ZoomIn();
		return 0;
	}
	break;

	case WM_KEYUP:													// Update Keyboard Buffers For Keys Released
// 		if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
// 		{
// 			m_keys.keyDown[wParam] = FALSE;					// Set The Selected Key (wParam) To False
// 			return 0;												// Return
// 		}
		break;															// Break

	case WM_TOGGLEFULLSCREEN:										// Toggle FullScreen Mode On/Off
		mbFullScreen = (mbFullScreen == TRUE) ? FALSE : TRUE;
		PostMessage(mhWnd, WM_QUIT, 0, 0);
		break;															// Break
	}

	return DefWindowProc(mhWnd, msg, wParam, lParam);					// Pass Unhandled Messages To DefWindowProc
}

void OglApp::SetMessagePumpReady(BOOL b)
{
	mbRunning = b;
}

void OglApp::Run()
{
	while (mbRunning)
	{
		if (PeekMessage(&mMsg, mhWnd, 0, 0, PM_REMOVE) != 0)
		{
			if (mMsg.message != WM_QUIT)
			{
				TranslateMessage(&mMsg);
				DispatchMessage(&mMsg);
			}
			else
			{
				mbRunning = FALSE;
			}
		}
		else //Idle
		{
			if (!mbVisible)
				WaitMessage();
			else
			{
				// Update
				double currentTime = clock();
				mUpdateTimeGap += currentTime - mLastTime;
				if (mUpdateTimeGap > mMinUpdateTimeGap)
				{
					Update(mMinUpdateTimeGap);
					mUpdateTimeGap -= mMinUpdateTimeGap;
				}
				mLastTime = currentTime;
				// Draw things
				ShowFps();
				Render();
				SwapBuffers(mhDC);
			}
		}
	}
	DestroyApp();
	return;
}

void OglApp::Resize(int w, int h)
{
	mWidth = w;
	mHeight = h;
	std::cout << "Resizing..\n";
}

void OglApp::Update(double deltaTime)
{
	ShowFps();
}

void OglApp::InitScene()
{
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
}

void OglApp::Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OglApp::ShowFps(void)
{
	static int frameCnt = 0;
	static double lt = clock();
	if (clock() - lt > 1000.0)
	{
		std::wostringstream ss;
		ss << mTitle << " FPS: " << frameCnt;
		std::wstring title(ss.str());
		SetWindowText(mhWnd, title.c_str());
		frameCnt = 0;
		lt = clock();
	}
	else
	{
		++frameCnt;
	}
}

void OglApp::SetVSync(bool bSync)
{
	// Function pointer for the wgl extention function we need to enable/disable
	// vsync
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	const char *extensions = (char*)glGetString(GL_EXTENSIONS);

	if (strstr(extensions, "WGL_EXT_swap_control") == 0)
	{
		std::cout << "VSync Unsupport!\n";
		return;
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
		std::cout << "VSync Support!\n";
		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(bSync);
	}
}



