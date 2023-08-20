#pragma once
#include "../Utils/Vec3.h"

using namespace Hydro;

float signedDistanceSphere( Vec3F point, Vec3F sphereOrigin, float radius );
float signedDistanceBox( Vec3F point, Vec3F boxOrigin, Vec3F boxSize );