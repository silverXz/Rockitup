#include "Camera.h"
#include <assert.h>

namespace iv
{
	Camera::Camera(vec3 p /*= vec3(0.0f,0.0f,-1.0f)*/, vec3 v /*= vec3(0.0f)*/, vec3 u /*= vec3(0.0f,1.0f,0.0f)*/, float n /*= 0.1f*/, float f /*= 1000.0f*/, float ratio /*= 1.75f*/,float fovy /*=90.0f*/)
	{
		mNear = n;
		mFar = f;
		mRatio = ratio;
		mFovy = fovy;
		mFocusLength = 1.0f;
		mPosition = p;
		
		mFaceOrientation = normalize(v - p);
		
		vec3 right = cross(mFaceOrientation, u);

		mUpOrientation = normalize(cross(right, mFaceOrientation));
	}

	Camera::~Camera()
	{
	}

	mat4 Camera::GetViewMatrix() const
	{
		return lookAt(mPosition, mPosition + mFaceOrientation, mUpOrientation);
	}

	mat4 Camera::GetProjectionMatrix() const
	{
		return perspective(mFovy, mRatio, mNear, mFar);
	}

	void Camera::SetFrustrum(float fovy, float ratio, float n, float f)
	{
		assert(n < f);
		mFovy = fovy;
		mRatio = ratio;
		mNear = n;
		mFar = f;
	}



}