#ifndef _FPSCONTROLLER_H_
#define _FPSCONTROLLER_H_

#include "Controller.h"
#include "SiMath.h"
#include <windows.h>

namespace iv
{
	class FPSController : public Controller
	{

	public:
		FPSController();
		~FPSController();

		virtual void HandleMouseEvent(const MouseEvent& eMouse,HWND hWnd);

		virtual void HandleKeyboardEvent(const KeyboardEvent& eKey);

		virtual void Update(double deltaT);


	private:
		float mCurMoveSpeed;
		float mCurMoveSpeed_X;
		float mCurRotateSpeed;


	public:
		static float damp;
		static vec3 maxMoveSpeed;
		static vec3 maxRotateSpeed;
	};
}
#endif