#ifndef _EVENT_H_
#define _EVENT_H_

namespace iv
{
	enum EventSource
	{
		Src_None = 0,
		KeyBoard,
		Mouse,
		JoyStick
	};

	enum MouseButton
	{
		Mb_None = 0,
		Left,
		Right,
		Middle,
		MouseButtonNum
	};

	enum KeyboardButton
	{
		Kb_None = 0,
		W, A, S, D,
		ArrowUp, ArrowDown, ArrowLeft, ArrowRight,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
		KeyboardButtonNum
	};

	enum ButtonState
	{
		Bs_None = 0, // It's "Move" For Mouse Event.
		Pressed,
		Released
	};

	class MouseEvent
	{
	public:
		MouseButton mButton;
		ButtonState mState;
		int mx;
		int my;

		MouseEvent(MouseButton mb = Mb_None, ButtonState bs = Bs_None,int x = 0,int y = 0) :
			mButton(mb), mState(bs), mx(x), my(y) {}
	};

	class KeyboardEvent
	{
	public:
		KeyboardButton mKey;
		ButtonState mState;
		KeyboardEvent(KeyboardButton kb = Kb_None, ButtonState bs = Bs_None) :
			mKey(kb), mState(bs) {}
	};
}

#endif