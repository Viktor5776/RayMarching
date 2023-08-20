#include "Scene.h"

float signedDistanceSphere( Vec3F point, Vec3F sphereOrigin, float radius )
{
	return Vec3F::Distance( point, sphereOrigin ) - radius;
}
