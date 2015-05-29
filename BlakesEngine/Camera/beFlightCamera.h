#pragma once
#include "Math\beMath.h"

class beGamepad;

class beFlightCamera
{
	public:
		beFlightCamera();
		~beFlightCamera();

		void AttachGamepad(beGamepad* gamepad);
		void DetachGamepad();
		
		void Update(float dt);
		const Matrix& GetViewMatrix() const; // Can get position with beMath::PositionFromMatrix
		const Vec3& GetPosition() const; // Can get position with beMath::PositionFromMatrix

	private:
		Matrix m_viewMatrix;
		Matrix m_orientationMatrix;
		Vec3 m_position;
		beGamepad* m_gamepad;
		float m_yaw;
		float m_pitch;
		float m_roll;

};
