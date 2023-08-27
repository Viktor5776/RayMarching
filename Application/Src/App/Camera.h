#pragma once

#include "../Utils/Matrix.h"
#include <vector>
#include "../Win/Window.h"

using namespace Hydro;

class Camera
{
public:
	Camera( float verticalFOV, float nearClip, float farClip );

	bool OnUpdate( Window& wnd, float dt );
	void OnResize( uint32_t width, uint32_t height );

	const Matrix4F& GetProjection() const { return projection; }
	const Matrix4F& GetInverseProjection() const { return inverseProjection; }
	const Matrix4F& GetView() const { return view; }
	const Matrix4F& GetInverseView() const { return inverseView; }

	const Vec3F& GetPosition() const { return position; }
	const Vec3F& GetDirection() const { return forwardDirection; }

	float GetRotationSpeed();
private:
	void RecalculateProjection();
	void RecalcutateView();
private:
	Matrix4F projection;
	Matrix4F view;
	Matrix4F inverseProjection;
	Matrix4F inverseView;

	float verticalFOV = 45.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;

	Vec3F position{ 0.0f, 0.0f, 0.0f };
	Vec3F forwardDirection{ 0.0f, 0.0f, 0.0f };

	Vec3F forwardDirectionT{ 0.0f, 0.0f, 0.0f };

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};