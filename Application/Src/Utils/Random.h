#pragma once
#include <random>
#include <limits>
#include "Vec3.h"

namespace Hydro
{
	class Random
	{
	public:
		static void Init()
		{
			s_RandomEngine.seed( std::random_device()() );
		}

		static uint32_t UInt()
		{
			return s_Distribution( s_RandomEngine );
		}

		static uint32_t UInt( uint32_t min, uint32_t max )
		{
			return min + (s_Distribution( s_RandomEngine ) % (max - min + 1));
		}

		static float Float()
		{
			return (float)s_Distribution( s_RandomEngine ) / (float)(std::numeric_limits<uint32_t>::max)();
		}

		static Vec3F Vec3()
		{
			return Vec3F( Float(), Float(), Float() );
		}

		static Vec3F Vec3( float min, float max )
		{
			return Vec3F( Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min );
		}
	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};
}