#include "Camera.h"
#include "../Utils/Quaternion.h"


Camera::Camera( float verticalFOV, float nearClip, float farClip )
	:
	verticalFOV( verticalFOV ),
	nearClip( nearClip ),
	farClip( farClip )
{
	forwardDirection = Vec3F( 0.0f, 0.0f, -1.0f );
	position = Vec3F( 0.0f, 0.0f, 15.0f );

	RecalculateProjection();
	RecalcutateView();
}

bool Camera::OnUpdate( Window& wnd, float dt )
{
	Vec2F delta = Vec2F( 0.0f, 0.0f );

	while( const auto d = wnd.mouse.ReadRawDelta() )
	{
		delta.x += d->x;
		delta.y += d->y;
	}

	if( !wnd.mouse.RightIsPressed() )
	{
		wnd.EnableCursor();
		return false;
	}

	wnd.DisableCursor();

	bool moved = false;

	Vec3F upDirection( 0.0f, 1.0f, 0.0f );
	Vec3F rightDirection = Vec3F::Cross( forwardDirection, upDirection );

	float speed = 5.0f;

	if( wnd.keyboard.KeyIsPressed( 'G' ) )
	{
		delta.x += 1;
	}

	if( wnd.keyboard.KeyIsPressed( 'W' ) )
	{
		position += forwardDirection * speed * dt;
		moved = true;
	}
	else if( wnd.keyboard.KeyIsPressed( 'S' ) )
	{
		position -= forwardDirection * speed * dt;
		moved = true;
	}
	if( wnd.keyboard.KeyIsPressed( 'A' ) )
	{
		position -= rightDirection * speed * dt;
		moved = true;
	}
	else if( wnd.keyboard.KeyIsPressed( 'D' ) )
	{
		position += rightDirection * speed * dt;
		moved = true;
	}
	if( wnd.keyboard.KeyIsPressed( 'Q' ) )
	{
		position -= upDirection * speed * dt;
		moved = true;
	}
	else if( wnd.keyboard.KeyIsPressed( 'E' ) )
	{
		position += upDirection * speed * dt;
		moved = true;
	}

	if( delta.x != 0.0f || delta.y != 0.0f )
	{
		float pitchDelta = delta.y * GetRotationSpeed();
		float yawDelta = delta.x * GetRotationSpeed();

		Quaternion pitch = Quaternion::angleAxis( -pitchDelta, rightDirection );
		Quaternion yaw = Quaternion::angleAxis( -yawDelta, upDirection );

		Quaternion product = Quaternion::Cross( pitch, yaw );

		Quaternion rotation = Quaternion::Normalize( product );

		forwardDirection = Quaternion::Rotate( forwardDirection, rotation );
		forwardDirectionT = forwardDirection;
		moved = true;
	}

	if( moved )
	{
		RecalcutateView();
	}

	return moved;
}

void Camera::OnResize( uint32_t width, uint32_t height )
{
	if( width == m_ViewportWidth && height == m_ViewportHeight )
		return;

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	RecalculateProjection();
}

float Camera::GetRotationSpeed()
{
	return 0.001f;
}

void Camera::RecalculateProjection()
{
	projection = Matrix4F::PerspectiveFov( verticalFOV, (float)m_ViewportWidth, (float)m_ViewportHeight, nearClip, farClip );
	inverseProjection = Matrix4F::Inverse( projection );
}

void Camera::RecalcutateView()
{
	view = Matrix4F::LookAt( position, position + forwardDirection, Vec3F( 0.0f, 1.0f, 0.0f ) );
	inverseView = Matrix4F::Inverse( view );
}