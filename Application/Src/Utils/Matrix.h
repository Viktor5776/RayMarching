#pragma once
#include "Vec4.h"
#include <numbers>

namespace Hydro
{
	template <typename T>
	class Matrix4
	{
	public:
		Matrix4() { 

			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					elements[i][j] = (T)0;
				}
			}
		}
		Matrix4( const Matrix4<T>& rhs ) { 

			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					elements[i][j] = rhs.elements[i][j];
				}
			}
		}

		bool operator==( const Matrix4<T>& rhs ) const {
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					if( elements[i][j] != rhs.elements[i][j] )
					{
						return false;
					}
				}
			}
			return true;
		}

		T& operator[]( size_t index ) {
			return elements[index / 4][index % 4];
		}
		 
		//Matrix matrix addition
		Matrix4<T> operator+( const Matrix4<T>& rhs ) const {
			Matrix4<T> result;
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					result.elements[i][j] = elements[i][j] + rhs.elements[i][j];
				}
			}
			return result;
		}
		Matrix4<T>& operator+=( const Matrix4<T>& rhs ) {
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					elements[i][j] += rhs.elements[i][j];
				}
			}
			return *this;
		}

		//Matrix scalar addition
		Matrix4<T> operator+( const T& rhs ) const {
			Matrix4<T> result;
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					result.elements[i][j] = elements[i][j] + rhs;
				}
			}
			return result;
		}
		Matrix4<T> operator+=( const T& rhs ) {
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					elements[i][j] += rhs;
				}
			}
			return *this;
		}

		//Matrix matrix subtraction
		Matrix4<T> operator-( const Matrix4<T>& rhs ) const {
			Matrix4<T> result;
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					result.elements[i][j] = elements[i][j] - rhs.elements[i][j];
				}
			}
			return result;
		}
		Matrix4<T>& operator-=( const Matrix4<T>& rhs ) {
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					elements[i][j] -= rhs.elements[i][j];
				}
			}
			return *this;
		}

		//Matrix scalar subtraction
		Matrix4<T> operator-( const T& rhs ) const {
			Matrix4<T> result;
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					result.elements[i][j] = elements[i][j] - rhs;
				}
			}
			return result;
		}
		Matrix4<T> operator-=( const T& rhs ) {
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					elements[i][j] -= rhs;
				}
			}
			return *this;
		}

		//Matrix matrix multiplication
		Matrix4<T> operator*( const Matrix4<T>& rhs ) const {
			Matrix4<T> result;
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					for( size_t k = 0; k < 4; ++k )
					{
						result.elements[i][j] += elements[i][k] * rhs.elements[k][j];
					}
				}
			}
			return result;
		}
		Matrix4<T>& operator*=( const Matrix4<T>& rhs ) {
			Matrix4<T> result;
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					for( size_t k = 0; k < 4; ++k )
					{
						result.elements[i][j] += elements[i][k] * rhs.elements[k][j];
					}
				}
			}
			*this = result;
			return *this;
		}

		//Matrix scalar multiplication
		
		Matrix4<T> operator*( const T& rhs ) const {
			Matrix4<T> result;
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					result.elements[i][j] = elements[i][j] * rhs;
				}
			}
			return result;
		}
		Matrix4<T> operator*=( const T& rhs ) {
			for( size_t i = 0; i < 4; ++i )
			{
				for( size_t j = 0; j < 4; ++j )
				{
					elements[i][j] *= rhs;
				}
			}
			return *this;
		}

		template<typename T>
		static Matrix4<T> Inverse( Matrix4<T> m )
		{
			Matrix4<T> result;
			T inv[16], det;
			int i;

			inv[0] = m[5] * m[10] * m[15] -
				m[5] * m[11] * m[14] -
				m[9] * m[6] * m[15] +
				m[9] * m[7] * m[14] +
				m[13] * m[6] * m[11] -
				m[13] * m[7] * m[10];

			inv[4] = -m[4] * m[10] * m[15] +
				m[4] * m[11] * m[14] +
				m[8] * m[6] * m[15] -
				m[8] * m[7] * m[14] -
				m[12] * m[6] * m[11] +
				m[12] * m[7] * m[10];

			inv[8] = m[4] * m[9] * m[15] -
				m[4] * m[11] * m[13] -
				m[8] * m[5] * m[15] +
				m[8] * m[7] * m[13] +
				m[12] * m[5] * m[11] -
				m[12] * m[7] * m[9];

			inv[12] = -m[4] * m[9] * m[14] +
				m[4] * m[10] * m[13] +
				m[8] * m[5] * m[14] -
				m[8] * m[6] * m[13] -
				m[12] * m[5] * m[10] +
				m[12] * m[6] * m[9];

			inv[1] = -m[1] * m[10] * m[15] +
				m[1] * m[11] * m[14] +
				m[9] * m[2] * m[15] -
				m[9] * m[3] * m[14] -
				m[13] * m[2] * m[11] +
				m[13] * m[3] * m[10];

			inv[5] = m[0] * m[10] * m[15] -
				m[0] * m[11] * m[14] -
				m[8] * m[2] * m[15] +
				m[8] * m[3] * m[14] +
				m[12] * m[2] * m[11] -
				m[12] * m[3] * m[10];

			inv[9] = -m[0] * m[9] * m[15] +
				m[0] * m[11] * m[13] +
				m[8] * m[1] * m[15] -
				m[8] * m[3] * m[13] -
				m[12] * m[1] * m[11] +
				m[12] * m[3] * m[9];

			inv[13] = m[0] * m[9] * m[14] -
				m[0] * m[10] * m[13] -
				m[8] * m[1] * m[14] +
				m[8] * m[2] * m[13] +
				m[12] * m[1] * m[10] -
				m[12] * m[2] * m[9];

			inv[2] = m[1] * m[6] * m[15] -
				m[1] * m[7] * m[14] -
				m[5] * m[2] * m[15] +
				m[5] * m[3] * m[14] +
				m[13] * m[2] * m[7] -
				m[13] * m[3] * m[6];

			inv[6] = -m[0] * m[6] * m[15] +
				m[0] * m[7] * m[14] +
				m[4] * m[2] * m[15] -
				m[4] * m[3] * m[14] -
				m[12] * m[2] * m[7] +
				m[12] * m[3] * m[6];

			inv[10] = m[0] * m[5] * m[15] -
				m[0] * m[7] * m[13] -
				m[4] * m[1] * m[15] +
				m[4] * m[3] * m[13] +
				m[12] * m[1] * m[7] -
				m[12] * m[3] * m[5];

			inv[14] = -m[0] * m[5] * m[14] +
				m[0] * m[6] * m[13] +
				m[4] * m[1] * m[14] -
				m[4] * m[2] * m[13] -
				m[12] * m[1] * m[6] +
				m[12] * m[2] * m[5];

			inv[3] = -m[1] * m[6] * m[11] +
				m[1] * m[7] * m[10] +
				m[5] * m[2] * m[11] -
				m[5] * m[3] * m[10] -
				m[9] * m[2] * m[7] +
				m[9] * m[3] * m[6];

			inv[7] = m[0] * m[6] * m[11] -
				m[0] * m[7] * m[10] -
				m[4] * m[2] * m[11] +
				m[4] * m[3] * m[10] +
				m[8] * m[2] * m[7] -
				m[8] * m[3] * m[6];

			inv[11] = -m[0] * m[5] * m[11] +
				m[0] * m[7] * m[9] +
				m[4] * m[1] * m[11] -
				m[4] * m[3] * m[9] -
				m[8] * m[1] * m[7] +
				m[8] * m[3] * m[5];

			inv[15] = m[0] * m[5] * m[10] -
				m[0] * m[6] * m[9] -
				m[4] * m[1] * m[10] +
				m[4] * m[2] * m[9] +
				m[8] * m[1] * m[6] -
				m[8] * m[2] * m[5];

			det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

			if( det == 0 )
				return Matrix4<T>();

			det = (T)1.0 / det;

			for( i = 0; i < 16; i++ )
				result[i] = inv[i] * det;

			return result;

		}

		template <typename T>
		static Matrix4<T> LookAt( Vec3T<T> cameraPos, Vec3T<T> lookPos, Vec3T<T> up )
		{
			//Calculate forward axis direction
			Vec3T<T> forward = Vec3T<T>( cameraPos - lookPos ).Normalized();

			//Calculate right axis direction
			Vec3T<T> right = Vec3T<T>::Cross( up, forward ).Normalized();

			//Calculate up axis direction
			up = Vec3T<T>::Cross( forward, right ).Normalized();

			//Get translations
			T translationX = Vec3T<T>::Dot( cameraPos, right );
			T translationY = Vec3T<T>::Dot( cameraPos, up );
			T translationZ = Vec3T<T>::Dot( cameraPos, forward );

			//Create matrix
			Matrix4<T> result;
			result.elements[0][0] = right.x;
			result.elements[0][1] = right.y;
			result.elements[0][2] = right.z;
			result.elements[0][3] = translationX;
			result.elements[1][0] = up.x;
			result.elements[1][1] = up.y;
			result.elements[1][2] = up.z;
			result.elements[1][3] = translationY;
			result.elements[2][0] = forward.x;
			result.elements[2][1] = forward.y;
			result.elements[2][2] = forward.z;
			result.elements[2][3] = translationZ;
			result.elements[3][0] = (T)0;
			result.elements[3][1] = (T)0;
			result.elements[3][2] = (T)0;
			result.elements[3][3] = (T)1;

			return result;
		}

		template <typename T>
		static Matrix4<T> PerspectiveFov( T fov, T width, T height, T near_, T far_ )
		{
			Matrix4<T> Result;
			T yScale = (T)1.0 / (T)tan(std::numbers::pi/180.0 * fov / 2);
			T xScale = yScale / (width / height);
			T zRange = near_ - far_;

			Result.elements[0][0] = xScale;
			Result.elements[0][1] = (T)0;
			Result.elements[0][2] = (T)0;
			Result.elements[0][3] = (T)0;
			Result.elements[1][0] = (T)0;
			Result.elements[1][1] = yScale;
			Result.elements[1][2] = (T)0;
			Result.elements[1][3] = (T)0;
			Result.elements[2][0] = (T)0;
			Result.elements[2][1] = (T)0;
			Result.elements[2][2] = ( far_ + near_ ) / zRange;
			Result.elements[2][3] = -1;
			Result.elements[3][0] = (T)0;
			Result.elements[3][1] = (T)0;
			Result.elements[3][2] = 2 * far_ * near_ / zRange;
			Result.elements[3][3] = 0;

			return Result;
		}


	public:
		// [ row ][ col ]
		T elements[4][4];
	};

	template<typename T>
	Vec4T<T> operator*( const Matrix4<T>& left, const Vec4T<T>& right )
	{
		T result[4] = {0.0f};
		for( size_t i = 0; i < 4; ++i )
		{
			result[i] = left.elements[i][0] * right.x + left.elements[i][1] * right.y + left.elements[i][2] * right.z + left.elements[i][3] * right.w;
		}
		return Vec4T<T>( result[0], result[1], result[2], result[3] );
	}

	typedef Matrix4<float> Matrix4F;
	typedef Matrix4<double> Matrix4D;

}