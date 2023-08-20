#pragma once
#include <cmath>
#include "Vec2.h"

namespace Hydro
{
	class Quaternion;

	template<typename T>
	class Vec3T
	{
	public:
		//Constructors and operators
		Vec3T()
			:
			x( 0 ),
			y( 0 ),
			z( 0 )
		{}
		Vec3T( T x, T y, T z )
			:
			x( x ),
			y( y ),
			z( z )
		{}
		Vec3T( Vec2T<T> xy, T z )
			:
			x( xy.x ),
			y( xy.y ),
			z( z )
		{}
		Vec3T( T val )
			:
			x( val ),
			y( val ),
			z( val )
		{}

		template<typename S>
		explicit Vec3T( const Vec3T<S>& src )
			:
			x( (T)src.x ),
			y( (T)src.y ),
			z( (T)src.z )
		{}
		template<typename S, typename P>
		explicit Vec3T( const Vec2T<S>& xy, P z )
			:
			x( (T)xy.x ),
			y( (T)xy.y ),
			z( (T)z )
		{}

		Vec3T<T> operator-( Vec3T<T> rhs )
		{
			return Vec3T<T>( x - rhs.x, y - rhs.y, z - rhs.z );
		}
		Vec3T<T>& operator-=( Vec3T<T> rhs )
		{
			return *this = *this - rhs;
		}

		Vec3T<T> operator*( T rhs )
		{
			return Vec3T<T>( x * rhs, y * rhs, z * rhs );
		}

		Vec3T<T>& operator*=( T rhs )
		{
			return *this = *this * rhs;
		}

		Vec3T<T> operator/( T rhs )
		{
			return Vec3T<T>( x / rhs, y / rhs, z / rhs );
		}
		Vec3T<T>& operator/=( T rhs )
		{
			return *this = *this / rhs;
		}

		Vec3T<T> operator+( Vec3T<T> rhs )
		{
			return Vec3T<T>( x + rhs.x, y + rhs.y, z + rhs.z );
		}
		Vec3T<T>& operator+=( Vec3T<T> rhs )
		{
			return *this = *this + rhs;
		}

		bool operator==( Vec3T<T> rhs )
		{
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}

		bool operator!=( Vec3T<T> rhs )
		{
			return !(*this == rhs);
		}

	public:
		//Public Method
		void Set( T x_, T y_, T z_ )
		{
			x = x_;
			y = y_;
			z = z_;
		}

		void Normalize()
		{
			*this = Normalized();
		}

		T SqrMagnitude()
		{
			return (T)(x * x + y * y + z * z);
		}

		T Magnitude()
		{
			return (T)std::sqrt( SqrMagnitude() );
		}

		Vec3T<T> Normalized()
		{
			const T len = Magnitude();
			if( len != (T)0 )
			{
				return *this * ((T)1 / len);
			}
			return *this;
		}

	public:
		//Static Methods
		static T Dot( Vec3T<T> lhs, Vec3T<T> rhs )
		{
			return (T)(lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
		}

		static Vec3T<T> Cross( Vec3T<T> lhs, Vec3T<T> rhs )
		{
			return Vec3T<T>(
				lhs.y * rhs.z - lhs.z * rhs.y,
				lhs.z * rhs.x - lhs.x * rhs.z,
				lhs.x * rhs.y - lhs.y * rhs.x
			);
		}

		static T Distance( Vec3T<T> lhs, Vec3T<T> rhs )
		{
			T deltaX = lhs.x - rhs.x;
			T deltaY = lhs.y - rhs.y;
			T deltaZ = lhs.z - rhs.z;

			return (T)std::sqrt( deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ );
		}

		static Vec3T<T> Lerp( Vec3T<T> a, Vec3T<T> b, T t )
		{
			return a * (1 - t) + b * t;
		}

		static Vec3T<T> MoveTowards( Vec3T<T> point, Vec3T<T> target, T d )
		{
			T t = d / Vec3T::Distance( point, target );

			return point * (((1 - t) <= 0) ? 0 : (1 - t)) + target * t;
		}

		static Vec3T<T> Scale( Vec3T<T> lhs, Vec3T<T> rhs )
		{
			lhs.x *= rhs.x;
			lhs.y *= rhs.y;
			lhs.z *= rhs.z;

			return lhs;
		}

		static Vec3T<T> Clamp( Vec3T<T> src, Vec3T<T> min, Vec3T<T> max )
		{
			src.x = (src.x < min.x) ? min.x : ((src.x > max.x) ? max.x : src.x);
			src.y = (src.y < min.y) ? min.y : ((src.y > max.y) ? max.y : src.y);
			src.z = (src.z < min.z) ? min.z : ((src.z > max.z) ? max.z : src.z);

			return src;
		}

		static Vec3T<T> Abs( Vec3T<T> lhs )
		{
			Vec3T<T> result;

			result.x = abs( lhs.x );
			result.y = abs( lhs.y );
			result.z = abs( lhs.z );

			return result;
		}
	
		static Vec3T<T> Max( Vec3T<T> lhs, Vec3T<T> rhs )
		{
			lhs.x = (lhs.x > rhs.x) ? lhs.x : rhs.x;
			lhs.y = (lhs.y > rhs.y) ? lhs.y : rhs.y;
			lhs.z = (lhs.z > rhs.z) ? lhs.z : rhs.z;

			return lhs;
		}

		static Vec3T<T> Min( Vec3T<T> lhs, Vec3T<T> rhs )
		{
			lhs.x = (lhs.x > rhs.x) ? rhs.x : lhs.x;
			lhs.y = (lhs.y > rhs.y) ? rhs.y : lhs.y;
			lhs.z = (lhs.z > rhs.z) ? rhs.z : lhs.z;

			return lhs;
		}

		static Vec3T<T> Reflect( Vec3T<T> inDirection, Vec3T<T> inNormal )
		{
			return inDirection - inNormal * Dot( inDirection, inNormal ) * 2;
		}

	public:
		T x;
		T y;
		T z;
	};

	typedef Vec3T<float> Vec3F;
	typedef Vec3T<int> Vec3I;


	template<typename T>
	Vec3T<T> operator+( const Vec3T<T> lhs, const Vec3T<T> rhs )
	{
		return Vec3T<T>( lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z );
	}
	template<typename T>
	Vec3T<T> operator/( const Vec3T<T> lhs, T rhs )
	{
		return Vec3T<T>( lhs.x / rhs, lhs.y / rhs, lhs.z / rhs );
	}
	template<typename T>
	Vec3T<T> operator*( const Vec3T<T> lhs, T rhs )
	{
		return Vec3T<T>( lhs.x * rhs, lhs.y * rhs, lhs.z * rhs );
	}
	template<typename T>
	Vec3T<T> operator*( T rhs, const Vec3T<T> lhs )
	{
		return Vec3T<T>( lhs.x * rhs, lhs.y * rhs, lhs.z * rhs );
	}
	template<typename T>
	Vec3T<T> operator-( const Vec3T<T> lhs, const Vec3T<T> rhs )
	{
		return Vec3T<T>( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z );
	}
}