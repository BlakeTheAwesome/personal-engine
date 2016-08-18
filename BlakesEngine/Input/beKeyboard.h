#pragma once
#include "BlakesEngine/Core/beTypes.h"
#include "BlakesEngine/External/DirectXTK/Keyboard.h"

class beSystemEventManager;

class beKeyboard
{
public:
	using Button = DirectX::Keyboard::Keys;

	beKeyboard() = default;
	~beKeyboard();

	void Init(beSystemEventManager* systemEventManager);
	void Deinit();

	void Update(float dt);

	bool IsPressed(Button button) const;
	bool IsReleased(Button button) const;
	bool IsDown(Button button) const;

private:
	beSystemEventManager* m_systemEventManager = nullptr;
	DirectX::Keyboard::KeyboardStateTracker m_tracker;
};

