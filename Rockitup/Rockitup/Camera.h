#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Moveable.h"


namespace iv
{
	class Camera : public Moveable
	{
	public:
		Camera( vec3 p = vec3(0.0f,0.0f,10.0f),vec3 v = vec3(0.0f),vec3 u = vec3(0.0f,1.0f,0.0f),float n = 0.1f,float f = 1000.0f, float ratio = 1.75f,float fovy = 90.0f);
		~Camera();
	

		void SetFrustrum(float fovy, float ratio, float n, float f);

		void SetFocusLength(float l) { mFocusLength = l; }

		mat4 GetViewMatrix() const;
		mat4 GetProjectionMatrix() const;
		
	private:
		float mNear;
		float mFar;
		float mRatio;
		float mFovy;
		float mFocusLength;


	};
}

#endif