module;

#include "BlakesEngine/Platform/beWindows.h"
#include "BlakesEngine/External/DirectXTK/Keyboard.h"

export module beKeyboard;
import beTypes;
import beSystemEventManager;

// Brought in from DirectX::Keyboard::KeyboardStateTracker as constructor is failing to export from module - VC 16.9.3
class StateTracker
{
	using State = DirectX::Keyboard::State;
	using Keys = DirectX::Keyboard::Keys;

	public:
	State released;
	State pressed;

	StateTracker() { Reset(); }

	void Update(const State& state)
	{
		auto currPtr = reinterpret_cast<const uint32_t*>(&state);
		auto prevPtr = reinterpret_cast<const uint32_t*>(&lastState);
		auto releasedPtr = reinterpret_cast<uint32_t*>(&released);
		auto pressedPtr = reinterpret_cast<uint32_t*>(&pressed);
		for (size_t j = 0; j < (256 / 32); ++j)
		{
			*pressedPtr = *currPtr & ~(*prevPtr);
			*releasedPtr = ~(*currPtr) & *prevPtr;

			++currPtr;
			++prevPtr;
			++releasedPtr;
			++pressedPtr;
		}

		lastState = state;
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}

	bool IsKeyPressed(Keys key) const { return pressed.IsKeyDown(key); }
	bool IsKeyReleased(Keys key) const { return released.IsKeyDown(key); }
	State GetLastState() const { return lastState; }

	public:
	State lastState;
};

export class beKeyboard
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
	StateTracker m_tracker;
};

