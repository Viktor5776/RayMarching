#pragma once
#include <chrono>


namespace Hydro
{

	class Timer
	{
	public:
		Timer() noexcept
		{
			last = std::chrono::steady_clock::now();
		}
		float Mark() noexcept
		{
			const auto old = last;
			last = std::chrono::steady_clock::now();
			return std::chrono::duration<float>( last - old ).count();
		}
		float Peek() const noexcept
		{
			return std::chrono::duration<float>( std::chrono::steady_clock::now() - last ).count();
		}
	private:
		std::chrono::steady_clock::time_point last;
	};

}