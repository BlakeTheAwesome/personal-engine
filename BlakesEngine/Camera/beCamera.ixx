export module beCamera;
import beMath;

export class beCamera
{
	public:
		void SetPosition(const Vec3& pos);
		void SetRotation(const Vec3& rot);

		const Vec3& GetPosition() const;
		const Vec3& GetRotation() const;
		
		void Update();
		const Matrix& GetViewMatrix() const;

	private:
		Vec3 m_pos{0.0f, 0.0f, 3.0f};
		Vec3 m_rot{0.0f, 0.f, -1.0f};
		Matrix m_matrix;
};
