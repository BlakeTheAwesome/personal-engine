#pragma once
#include "BlakesEngine\Core/beTypes.h"

class beGamepad
{
public:
	enum Button
	{
		A,
		B,
		X,
		Y,
		L1,
		L2,
		R1,
		R2,
		Select,
		Start,
		Up,
		Down,
		Left,
		Right,

		ButtonCount,
	};

	beGamepad() = default;
	~beGamepad() = default;

	bool Init(char index);
	void Deinit();

	void Update(float dt);
	
	bool IsConnected() const;

	float GetLeftX() const;
	float GetLeftY() const;
	float GetRightX() const;
	float GetRightY() const;
	
	float GetL2() const;
	float GetR2() const;

	bool GetPressed(Button button) const;
	bool GetButtonReleased(Button button) const;

private:
	float m_leftX = 0.f;
	float m_leftY = 0.f;
	float m_rightX = 0.f;
	float m_rightY = 0.f;
	float m_l2 = 0.f;
	float m_r2 = 0.f;

	u32 m_lastPacketNumber = 0;
	u32 m_lastStateFlags = 0;
	u32 m_thisStateFlags = 0;
	char m_index = -1;
	bool m_connected = false;
};

