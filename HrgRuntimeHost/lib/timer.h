#pragma once 
#include <stdint.h>
#include <chrono>

namespace aristocrat
{
	class timer
	{
	private:
		
	
		std::chrono::high_resolution_clock::time_point _start;
	public:
		timer();
		virtual ~timer();
		
		void reset();
		double getElapsedTimeInSec();
		double getElapsedTimeInMilliSec();
		double getElapsedTimeInMicroSec();
		template <class... T> 
		inline double getElapsedTime()
		{
			using microseconds = std::chrono::duration<double, T...>;
			auto now = std::chrono::high_resolution_clock::now();
			auto elapsed = now - _start;
			_start = now;
			return std::chrono::duration_cast<microseconds>(elapsed).count();
		}
		

	};
}

