#include "Quaternion.h"
#include <numbers>
#include <math.h>

namespace Hydro
{
	Quaternion::Quaternion()
		:
		w(0.f),
		x(0.f),
		y(0.f),
		z(0.f)
	{}

	Quaternion::Quaternion( float w, float x, float y, float z )
		:
		w(w),
		x(x),
		y(y),
		z(z)
	{}

	Quaternion Quaternion::angleAxis( float angle, Vec3F axis )
	{
		Quaternion result;

		result.w = cos( angle / 2 );
		result.x = axis.x * sin( angle / 2 );
		result.y = axis.y * sin( angle / 2 );
		result.z = axis.z * sin( angle / 2 );

		return result;
	}

	Quaternion Quaternion::Normalize( Quaternion q )
	{
		Quaternion result;
		
		//Get length of quaternion
		float length = sqrt( q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z );

		if( length <= 0.0f )
			return Quaternion();

		//Normalize quaternion
		float oneOverLength = 1.0f / length;
		result.w = q.w * oneOverLength;
		result.x = q.x * oneOverLength;
		result.y = q.y * oneOverLength;
		result.z = q.z * oneOverLength;

		return result;
	}

	Quaternion Quaternion::Cross( Quaternion q1, Quaternion q2 )
	{
		return Quaternion(
				q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z, 
				q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y, 
				q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z, 
				q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x );

		return Quaternion();
	}

	Vec3F Quaternion::Rotate( Vec3F vec, Quaternion q )
	{
		Vec3F QuatVector( q.x, q.y, q.z );
		Vec3F uv = Vec3F::Cross( QuatVector, vec );
		Vec3F uuv = Vec3F::Cross( QuatVector, uv );

		return vec + ((uv * q.w) + uuv) * 2.0f;
	}

}