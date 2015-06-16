#pragma once

#include <windows.h>

#include <string>


#define WM_TOGGLEFULLSCREEN (WM_USER+1)

#define CHECK_FOR_MULTISAMPLE 1

typedef struct {
	BOOL keyDown[256];
} Keys;


class OglApp
{
public:
	OglApp(HINSTANCE hInstance,BOOL bShowConsole = TRUE);
	~OglApp();


	void InitApp(wchar_t* title, int x, int y, int width, int height, int bits = 32, bool fullsrceen = false, bool maximized = false, bool multisampled = false,bool bVSync = true);
	static void ShowWin32Console();
	void Resize(int w,int h);

	void ShowFps(void);

	void SetVSync(bool bSync);

	virtual void InitScene();
	virtual void Update(double deltaTime);
	virtual void Render(void);
	void Run();
	virtual LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
protected:

	BOOL ChangeScreenResolution(int width, int height, int bitsPerPixel);
	BOOL _InitApp(wchar_t* title, int x, int y, int width, int height, int bits, bool fullsrceen, bool maximized, bool multisampled, bool bVSync);
	BOOL InitOgl();
	BOOL DestroyApp();
	BOOL RegisterWindowClass();
	BOOL InitMultisample(HINSTANCE hInstance, HWND hWnd, PIXELFORMATDESCRIPTOR pfd);
	BOOL IsWglExtensionSupported(const char* extension);
	
	void Terminate();
	void SetMessagePumpReady(BOOL b);
	void ToggleFullScreen();
	
	
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	BOOL		mbRunning;
	BOOL		mbFullScreen;
	BOOL		mbMaximized;
	BOOL		mbMessageBumpping;
	BOOL		mbMultisample;

	double		mUpdateTimeGap;
	double		mMinUpdateTimeGap;

	BOOL		mbWglSupportMultisample;
	BOOL		mbVisible;

	double		mLastTime;

	int			mArbMultisamplePixelFormat;

	int			mPosX;
	int			mPosY;
	int			mWidth;
	int			mHeight;
	int			mBitsPerPixel;

	Keys		mKeys;

	MSG			mMsg;
	
	std::wstring	mTitle;

	HINSTANCE	mhInstance;
	HWND		mhWnd;
	HDC			mhDC;
	HGLRC		mhGlhc;


};
