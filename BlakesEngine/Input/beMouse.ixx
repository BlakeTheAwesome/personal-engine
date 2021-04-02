module;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

export module beMouse;

import beSystemEventManager;
import beTypes;
import beArray;
import beWindow;

export class beMouse
{
public:
	enum Mode
	{
		MODE_ABSOLUTE = 0,
		MODE_RELATIVE,
	};
	enum Button {
		LeftButton,
		RightButton,
		MiddleButton,
		Button4,
		Button5,
		Button6,
		Button7,
		Button8,

		ButtonCount
	};

	beMouse() = default;
	~beMouse();

	beMouse(const beMouse&) = delete;
	beMouse(beMouse&&) = delete;
	beMouse& operator=(const beMouse&) = delete;
	beMouse& operator=(beMouse&&) = delete;


	void Init(beSystemEventManager* systemEventManager, const beWindow* window);
	void Deinit();

	void Update(float dt);

	bool IsPressed(Button button) const;
	bool IsReleased(Button button) const;
	bool IsDown(Button button) const;

	// This is SCREEN relative - not window relative.
	int GetX() const { return m_currentState.x; };
	int GetY() const { return m_currentState.y; };
	int GetScrollWheel() const { return m_currentState.z; }

	int GetXMovement() const { return m_currentState.mouseState.lX; };
	int GetYMovement() const { return m_currentState.mouseState.lY; };
	int GetScrollWheelMovement() const { return m_currentState.mouseState.lZ; }

private:
	beSystemEventManager* m_systemEventManager = nullptr;
	beSystemEventManager::CallbackId m_systemCallbackId;

	struct State
	{
		int x{0};
		int y{0};
		int z{0};
		DIMOUSESTATE2 mouseState{};
	};
	

	IDirectInput8* m_directInput{nullptr};
	IDirectInputDevice8* m_mouse{nullptr};

	State m_currentState;
	State m_lastState;
	int m_windowWidth = 0;
	int m_windowHeight = 0;
	//Mode m_currentMode{MODE_RELATIVE};
};

