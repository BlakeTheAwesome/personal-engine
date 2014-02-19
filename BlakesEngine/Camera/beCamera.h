#pragma once
#include "Math\beMath.h"

class beCamera
{
	public:
		beCamera();
		~beCamera();

		void SetPosition(const Vec3& pos);
		void SetRotation(const Vec3& rot);

		const Vec3& GetPosition() const;
		const Vec3& GetRotation() const;
		
		void Update();
		const Matrix& GetViewMatrix() const;

	private:
		Vec3 m_pos;
		Vec3 m_rot;
		Matrix m_matrix;
};
