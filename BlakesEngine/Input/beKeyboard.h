#pragma once
#include "BlakesEngine/Platform/beWindows.h"
#include "BlakesEngine/External/DirectXTK/Keyboard.h"
import beTypes;

class beSystemEventManager;

class beKeyboard
{
public:
	using Button = DirectX::Keyboard::Keys;

	beKeyboard() = default;
	~beKeyboard();

	beKeyboard(const beKeyboard&) = delete;
	beKeyboard(beKeyboard&&) = delete;
	beKeyboard& operator=(const beKeyboard&) = delete;
	beKeyboard& operator=(beKeyboard&&) = delete;

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

