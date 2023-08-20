#include "Scene.h"

float signedDistanceSphere( Vec3F point, Vec3F sphereOrigin, float radius )
{
	return Vec3F::Distance( point, sphereOrigin ) - radius;
}

float signedDistanceBox( Vec3F point, Vec3F boxOrigin, Vec3F boxSize )
{
	Vec3F p = point - boxOrigin;
	Vec3F q = Vec3F::Abs( p ) - boxSize;
	return (Vec3F::Max( q, Vec3F( 0.0f ) )).Magnitude() + std::min( std::max( q.x, std::max( q.y, q.z ) ), 0.0f );
}
