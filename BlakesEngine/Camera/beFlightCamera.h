#pragma once
#include "BlakesEngine/Math/beMath.h"

class beGamepad;
class beMouse;

class beFlightCamera
{
	public:
		beFlightCamera();
		beFlightCamera(const beFlightCamera&) = default;
		beFlightCamera(beFlightCamera&&) = default;
		~beFlightCamera();

		beFlightCamera& operator=(const beFlightCamera&) = default;
		beFlightCamera& operator=(beFlightCamera&&) = default;

		void AttachGamepad(beGamepad* gamepad);
		void DetachGamepad();
		void AttachMouse(beMouse* mouse);
		void DetachMouse();
		
		void Update(float dt);
		const Matrix& GetViewMatrix() const { return m_viewMatrix; } // Can get position with beMath::PositionFromMatrix
		const Vec3& GetPosition() const { return m_position; }

		void SetPosition(const Vec3& v) { m_position = v; }
		void SetPitch(float f) { m_pitch = f; }
		void SetYaw(float f) { m_yaw = f; }
		void SetRoll(float f) { m_roll = f; }

	private:
		void UpdateImpl(float dt, float extraPitch, float extraYaw, float extraForwards, float extraRight);

		Matrix m_viewMatrix;
		Matrix m_orientationMatrix;
		Vec3 m_position{20.f, -30.f, 30.f};
		beGamepad* m_gamepad{nullptr};
		beMouse* m_mouse{nullptr};
		float m_yaw{-1.f};
		float m_pitch{-1.f};
		float m_roll{0.f};
};
