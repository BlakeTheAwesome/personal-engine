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
		const Matrix& GetViewMatrix() const;

	private:
		Matrix m_matrix;
		beGamepad* m_gamepad;
};
