#pragma once
#include <cmath>

namespace Hydro
{

	template<typename T>
	class Vec2T
	{
	public:
		//Constructors and operators
		Vec2T()
			:
			x( 0 ),
			y( 0 )
		{}
		Vec2T( T x, T y )
			:
			x( x ),
			y( y )
		{}
		template<typename S>
		explicit Vec2T( const Vec2T<S>& src )
			:
			x( (T)src.x ),
			y( (T)src.y )
		{}
		Vec2T( T val )
			:
			x( val ),
			y( val )
		{}

		Vec2T<T> operator-( Vec2T<T> rhs )
		{
			return Vec2T<T>( x - rhs.x, y - rhs.y );
		}
		Vec2T<T>& operator-=( Vec2T<T> rhs )
		{
			return *this = *this - rhs;
		}

		Vec2T<T> operator*( T rhs )
		{
			return Vec2T<T>( x * rhs, y * rhs );
		}
		Vec2T<T>& operator*=( T rhs )
		{
			return *this = *this * rhs;
		}

		Vec2T<T> operator/( T rhs )
		{
			return Vec2T<T>( x / rhs, y / rhs );
		}
		Vec2T<T>& operator/=( T rhs )
		{
			return *this = *this / rhs;
		}

		Vec2T<T> operator+( Vec2T<T> rhs )
		{
			return Vec2T<T>( x + rhs.x, y + rhs.y );
		}
		Vec2T<T>& operator+=( Vec2T<T> rhs )
		{
			return *this = *this + rhs;
		}

		bool operator==( Vec2T<T> rhs )
		{
			return x == rhs.x && y == rhs.y;
		}

		bool operator!=( Vec2T<T> rhs )
		{
			return x != rhs.x && y != rhs.y;
		}

	public:
		//Public Method
		void Set( T x_, T y_ )
		{
			x = x_;
			y = y_;
		}
			
		void Normalize()
		{
			*this = Normalized();
		}

		T SqrMagnitude()
		{
			return (T)(x * x + y * y);
		}

		T Magnitude()
		{
			return (T)std::sqrt( SqrMagnitude() );
		}

		Vec2T<T> Normalized()
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
		static T Dot( Vec2T<T> lhs, Vec2T<T> rhs )
		{
			return (T)(lhs.x * rhs.x + lhs.y * rhs.y);
		}

		static T Distance( Vec2T<T> lhs, Vec2T<T> rhs )
		{
			T deltaX = lhs.x - rhs.x;
			T deltaY = lhs.y - rhs.y;

			return (T)std::sqrt( deltaX * deltaX + deltaY * deltaY );
		}

		static Vec2T<T> Lerp( Vec2T<T> a, Vec2T<T> b, T t )
		{
			return a * (1 - t) + b * t;
		}

		static Vec2T<T> MoveTowards( Vec2T<T> point, Vec2T<T> target, T d )
		{
			T t = d / Vec2T::Distance( point, target );

			return point * (((1-t) <= 0) ? 0 : (1 - t)) + target * t;
		}
		
		static Vec2T<T> Scale( Vec2T<T> lhs, Vec2T<T> rhs )
		{
			lhs.x *= rhs.x;
			lhs.y *= rhs.y;

			return lhs;
		}

		static Vec2T<T> Clamp( Vec2T<T> src, Vec2T<T> min, Vec2T<T> max )
		{
			src.x = (src.x < min.x) ? min.x : ((src.x > max.x) ? max.x : src.x);
			src.y = (src.y < min.y) ? min.y : ((src.y > max.y) ? max.y : src.y);

			return src;
		}

	public:
		T x;
		T y;
	};

	typedef Vec2T<float> Vec2F;
	typedef Vec2T<int> Vec2I;

	template<typename T>
	Vec2T<T> operator+( const Vec2T<T> lhs, const Vec2T<T> rhs )
	{
		return Vec2T<T>( lhs.x + rhs.x, lhs.y + rhs.y );
	}
	template<typename T>
	Vec2T<T> operator/( const Vec2T<T> lhs, T rhs )
	{
		return Vec2T<T>( lhs.x / rhs, lhs.y / rhs );
	}
	template<typename T>
	Vec2T<T> operator*( const Vec2T<T> lhs, T rhs )
	{
		return Vec2T<T>( lhs.x * rhs, lhs.y * rhs );
	}
	template<typename T>
	Vec2T<T> operator-( const Vec2T<T> lhs, const Vec2T<T> rhs )
	{
		return Vec2T<T>( lhs.x - rhs.x, lhs.y - rhs.y );
	}
}