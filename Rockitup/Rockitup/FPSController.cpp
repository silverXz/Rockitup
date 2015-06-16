#include "FPSController.h"

namespace iv
{


	FPSController::FPSController() : mCurMoveSpeed(0.0f), mCurRotateSpeed(0.0f)
	{

	}

	FPSController::~FPSController()
	{

	}

	void FPSController::Update(double deltaT)
	{

		if (mKeyPressed[W])
			mCurMoveSpeed = maxMoveSpeed.y();

		if (mKeyPressed[S])
			mCurMoveSpeed = -maxMoveSpeed.y();

		if (mKeyPressed[A])
			mCurMoveSpeed_X = -maxMoveSpeed.x();

		if (mKeyPressed[D])
			mCurMoveSpeed_X = maxMoveSpeed.x();


		// Update Axis-Y Speed 
		if (mCurMoveSpeed < 0.0f)
		{
			mCurMoveSpeed += (float)(damp * deltaT);

			if (mCurMoveSpeed >= 0.0f)
				mCurMoveSpeed = 0.0f;
		}
		else if (mCurMoveSpeed > 0.0f)
		{
			mCurMoveSpeed -= (float)(damp * deltaT);
			if (mCurMoveSpeed <= 0.0f)
				mCurMoveSpeed = 0.0f;
		}



		// Update Axis-Y Position
		iv::vec3 d = iv::normalize(mpAvatar->GetFaceOrientation());
		iv::vec3 p = mpAvatar->GetPosition();

		p = p + d * mCurMoveSpeed;

		mpAvatar->SetPosition(p);


		// Update Axis-X Speed
		if (mCurMoveSpeed_X < 0.0f)
		{
			mCurMoveSpeed_X += (float)(damp * deltaT);

			if (mCurMoveSpeed_X >= 0.0f)
				mCurMoveSpeed_X = 0.0f;
		}
		else if (mCurMoveSpeed_X > 0.0f)
		{
			mCurMoveSpeed_X -= (float)(damp * deltaT);
			if (mCurMoveSpeed_X <= 0.0f)
				mCurMoveSpeed_X = 0.0f;
		}

		// Update Axis-X Position
		iv::vec3 right = normalize(cross(d, vec3(0.0f, 1.0f, 0.0f)));

		p = p + right * mCurMoveSpeed_X;
		mpAvatar->SetPosition(p);
	}

	void FPSController::HandleMouseEvent(const MouseEvent& eMouse, HWND hWnd)
	{
		if (mpAvatar.get() == 0)
			return;

		// Platform related Code.
		RECT rect;
		
		GetClientRect(hWnd,&rect);

		int centerX = ( rect.right - rect.left ) / 2;
		int centerY = ( rect.bottom - rect.top ) / 2;

		if (eMouse.mx == centerX && eMouse.my == centerY)
			return;

		switch (eMouse.mButton)
		{
		case Mb_None:
		{
			int dx = eMouse.mx - centerX;
			int dy = centerY - eMouse.my;

			mat4 yRot = rotate(-maxRotateSpeed.y() * (float)dx, vec3(0.0f, 1.0f, 0.0f));
			vec3 face = normalize(mpAvatar->GetFaceOrientation());
			face = mat3(yRot) * face;
			vec3 right = normalize(cross(face, vec3(0.0f, 1.0f, 0.0f)));
			mat4 xRot = rotate(maxRotateSpeed.x() * (float)dy, right);
			face = mat3(xRot) * face;
			vec3 up = normalize(cross(right, face));
			mpAvatar->SetFaceOrientation(face);
			mpAvatar->SetUpOrientation(up);
			break;
		}
		default:
			break;
		}
		
		ClientToScreen(hWnd,reinterpret_cast<POINT*>(&rect.left));

		SetCursorPos(rect.left + centerX, rect.top + centerY);
	}

	void FPSController::HandleKeyboardEvent(const KeyboardEvent& eKey)
	{
		if (mpAvatar.get() == 0)
			return;

		assert(eKey.mState != Bs_None);

		if (eKey.mState == Pressed)
			mKeyPressed[eKey.mKey] = true;
		else
			mKeyPressed[eKey.mKey] = false;
	}

	float	FPSController::damp = 0.005f;
	vec3	FPSController::maxMoveSpeed = vec3(1.0f,1.0f,0.0f);
	vec3	FPSController::maxRotateSpeed = vec3(HALFPI / 500.0f, HALFPI / 500.0f, 0.0f);// Not used for now!

}