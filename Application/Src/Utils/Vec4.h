#pragma once
#pragma once
#include <cmath>
#include "Vec3.h"

namespace Hydro
{

	template<typename T>
	class Vec4T
	{
	public:
		//Constructors and operators
		Vec4T()
			:
			x( 0 ),
			y( 0 ),
			z( 0 ),
			w( 0 )
		{}
		Vec4T( T x, T y, T z, T w )
			:
			x( x ),
			y( y ),
			z( z ),
			w( w )
		{}
		Vec4T( Vec3T<T> xyz, T w )
			:
			x( xyz.x ),
			y( xyz.y ),
			z( xyz.z ),
			w( w )
		{}
		Vec4T( Vec2T<T> xy, T z, T w )
			:
			x( xy.x ),
			y( xy.y ),
			z( z ),
			w( w )
		{}
		Vec4T( Vec2T<T> xy, Vec2T<T> zw )
			:
			x( xy.x ),
			y( xy.y ),
			z( zw.x ),
			w( zw.y )
		{}
		Vec4T( T val )
			:
			x( val ),
			y( val ),
			z( val ),
			w( val )
		{}

		template<typename S>
		explicit Vec4T( const Vec4T<S>& src )
			:
			x( (T)src.x ),
			y( (T)src.y ),
			z( (T)src.z ),
			w( (T)src.w )
		{}
		template<typename S, typename P>
		explicit Vec4T( const Vec3T<S>& xyz, P w )
			:
			x( (T)xyz.x ),
			y( (T)xyz.y ),
			z( (T)xyz.z ),
			w( (T)w )
		{}
		template<typename S, typename P, typename M>
		explicit Vec4T( const Vec2T<S>& xy, P z, M w )
			:
			x( (T)xy.x ),
			y( (T)xy.y ),
			z( (T)z ),
			w( (T)w )
		{}
		template<typename S, typename P>
		explicit Vec4T( const Vec2T<S>& xy, const Vec2T<P>& zw )
			:
			x( (T)xy.x ),
			y( (T)xy.y ),
			z( (T)zw.x ),
			w( (T)zw.y )
		{}

		
		Vec4T<T> operator-( Vec4T<T> rhs )
		{
			return Vec4T<T>( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w );
		}
		Vec4T<T>& operator-=( Vec4T<T> rhs )
		{
			return *this = *this - rhs;
		}

		
		Vec4T<T> operator*( T rhs )
		{
			return Vec4T<T>( x * rhs, y * rhs, z * rhs, w * rhs );
		}
		Vec4T<T>& operator*=( T rhs )
		{
			return *this = *this * rhs;
		}

		
		Vec4T<T> operator/( T rhs )
		{
			return Vec4T<T>( x / rhs, y / rhs, z / rhs, w / rhs );
		}
		Vec4T<T>& operator/=( T rhs )
		{
			return *this = *this / rhs;
		}

		
		Vec4T<T> operator+( Vec4T<T> rhs )
		{
			return Vec4T<T>( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w );
		}
		Vec4T<T>& operator+=( Vec4T<T> rhs )
		{
			return *this = *this + rhs;
		}

		bool operator==( Vec4T<T> rhs )
		{
			return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
		}

		bool operator!=( Vec4T<T> rhs )
		{
			return !(*this == rhs);
		}

	public:
		//Public Method
		void Set( T x_, T y_, T z_, T w_ )
		{
			x = x_;
			y = y_;
			z = z_;
			w = w_;
		}

		void Normalize()
		{
			*this = Normalized();
		}

		T SqrMagnitude()
		{
			return (T)(x * x + y * y + z * z + w * w);
		}

		T Magnitude()
		{
			return (T)std::sqrt( SqrMagnitude() );
		}

		Vec4T<T> Normalized()
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
		static T Dot( Vec4T<T> lhs, Vec4T<T> rhs )
		{
			return (T)(lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w);
		}

		static T Distance( Vec4T<T> lhs, Vec4T<T> rhs )
		{
			T deltaX = lhs.x - rhs.x;
			T deltaY = lhs.y - rhs.y;
			T deltaZ = lhs.z - rhs.z;
			T deltaW = lhs.w - rhs.w;

			return (T)std::sqrt( deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ + deltaW * deltaW );
		}

		static Vec4T<T> Lerp( Vec4T<T> a, Vec4T<T> b, T t )
		{
			return a * (1 - t) + b * t;
		}

		static Vec4T<T> MoveTowards( Vec4T<T> point, Vec4T<T> target, T d )
		{
			T t = d / Vec4T::Distance( point, target );

			return point * (((1 - t) <= 0) ? 0 : (1 - t)) + target * t;
		}

		static Vec4T<T> Scale( Vec4T<T> lhs, Vec4T<T> rhs )
		{
			lhs.x *= rhs.x;
			lhs.y *= rhs.y;
			lhs.z *= rhs.z;
			lhs.w *= rhs.w;

			return lhs;
		}

		static Vec4T<T> Clamp( Vec4T<T> src, Vec4T<T> min, Vec4T<T> max )
		{
			src.x = (src.x < min.x) ? min.x : ((src.x > max.x) ? max.x : src.x);
			src.y = (src.y < min.y) ? min.y : ((src.y > max.y) ? max.y : src.y);
			src.z = (src.z < min.z) ? min.z : ((src.z > max.z) ? max.z : src.z);
			src.w = (src.w < min.w) ? min.w : ((src.w > max.w) ? max.w : src.w);

			return src;
		}

	public:
		T x;
		T y;
		T z;
		T w;
	};

	typedef Vec4T<float> Vec4F;
	typedef Vec4T<int> Vec4I;

	template<typename T>
	Vec4T<T> operator+( const Vec4T<T> lhs, const Vec4T<T> rhs )
	{
		return Vec4T<T>( lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w );
	}
	template<typename T>
	Vec4T<T> operator/( const Vec4T<T> lhs, T rhs )
	{
		return Vec4T<T>( lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs );
	}
	template<typename T>
	Vec4T<T> operator*( const Vec4T<T> lhs, T rhs )
	{
		return Vec4T<T>( lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs );
	}
	template<typename T>
	Vec4T<T> operator-( const Vec4T<T> lhs, const Vec4T<T> rhs )
	{
		return Vec4T<T>( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w );
	}
}