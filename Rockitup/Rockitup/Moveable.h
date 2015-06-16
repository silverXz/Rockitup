#ifndef _MOVEABLE_H_
#define _MOVEABLE_H_

#include "SiMath.h"
#include "Controller.h"

namespace iv
{
	class Moveable
	{
	public:
		Moveable() {}
		virtual ~Moveable() {}

		void SetPosition(vec3& p)
		{
			mPosition = p;
		}

		void SetFaceOrientation(vec3& f)
		{
			mFaceOrientation = f;
		}

		void SetUpOrientation(vec3& u)
		{
			mUpOrientation = u;
		}

		vec3 GetPosition(void)
		{
			return mPosition;
		}

		vec3 GetFaceOrientation(void)
		{
			return mFaceOrientation;
		}

		vec3 GetUpOrientation(void)
		{
			return mUpOrientation;
		}

	protected:
		vec3 mPosition;
		vec3 mFaceOrientation;
		vec3 mUpOrientation;


	};
}

#endif