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

	beGamepad();
	~beGamepad();

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
	float m_leftX;
	float m_leftY;
	float m_rightX;
	float m_rightY;
	float m_l2;
	float m_r2;

	u32 m_lastPacketNumber;
	u32 m_lastStateFlags;
	u32 m_thisStateFlags;
	char m_index;
	bool m_connected;
};

