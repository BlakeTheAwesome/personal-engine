#pragma once
#include "BlakesEngine/Math/beMath.h"

class beGamepad;
class beMouse;

class beFlightCamera
{
	public:
		beFlightCamera();
		~beFlightCamera();

		void AttachGamepad(beGamepad* gamepad);
		void DetachGamepad();
		void AttachMouse(beMouse* mouse);
		void DetachMouse();
		
		void Update(float dt);
		const Matrix& GetViewMatrix() const; // Can get position with beMath::PositionFromMatrix
		const Vec3& GetPosition() const; // Can get position with beMath::PositionFromMatrix

	private:
		void UpdateImpl(float dt, float extraPitch, float extraYaw, float extraForwards, float extraRight);

		Matrix m_viewMatrix;
		Matrix m_orientationMatrix;
		Vec3 m_position{0.f, 0.f, -5.f};
		beGamepad* m_gamepad{nullptr};
		beMouse* m_mouse{nullptr};
		float m_yaw{0.f};
		float m_pitch{0.f};
		float m_roll{0.f};
};
