#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <memory>
#include "Event.h"
#include "Moveable.h"



namespace iv
{
	class Moveable;

	class Controller
	{
	protected:
		std::shared_ptr<Moveable> mpAvatar;
		bool mKeyPressed[KeyboardButtonNum];
		bool mMouseKeyPressed[MouseButtonNum];

	public:
		Controller() :mpAvatar(0) 
		{
			memset(mKeyPressed, 0, sizeof(bool) * KeyboardButtonNum);
			memset(mMouseKeyPressed, 0, sizeof(bool)*MouseButtonNum);
		}
		virtual ~Controller() {}


		virtual void Attach(std::shared_ptr<Moveable> pAvatar)
		{
			mpAvatar = pAvatar;
		}

		virtual void Update(double deltaT) {}

		virtual void HandleMouseEvent(const MouseEvent& eMouse) {}

		virtual void HandleKeyboardEvent(const KeyboardEvent& eKey) {}

	};
}
#endif