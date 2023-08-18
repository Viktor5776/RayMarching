#pragma once
#include "Vec4.h"

namespace Hydro
{
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion( float w, float x, float y, float z );
		static Quaternion angleAxis( float angle, Vec3F axis );
		static Quaternion Normalize( Quaternion q );
		static Quaternion Cross( Quaternion q1, Quaternion q2 );
		static Vec3F Rotate( Vec3F vec, Quaternion q );
	public:
		float w;
		float x;
		float y;
		float z;
	};
}