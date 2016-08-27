#pragma once
#include "BlakesEngine/Core/beTypes.h"
#include "BlakesEngine/External/DirectXTK/Mouse.h"

class beSystemEventManager;
class beWindow;

class beMouse
{
public:
	using Mode = DirectX::Mouse::Mode;

	enum class Button {
		LeftButton,
		MiddleButton,
		RightButton,
		XButton1,
		XButton2,
	};

	beMouse() = default;
	~beMouse();

	void Init(beSystemEventManager* systemEventManager, const beWindow* window);
	void Deinit();

	void Update(float dt);

	bool IsPressed(Button button) const;
	bool IsReleased(Button button) const;
	bool IsDown(Button button) const;

	int GetX() const;
	int GetY() const;
	int GetScrollWheelValue() const;

private:
	beSystemEventManager* m_systemEventManager = nullptr;
	DirectX::Mouse::ButtonStateTracker m_tracker;
};

