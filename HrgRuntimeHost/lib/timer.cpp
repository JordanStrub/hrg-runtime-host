#include "timer.h"
#include <windows.h>

#pragma comment(lib, "winmm.lib")

namespace aristocrat
{
	timer::timer()
	{
		timeBeginPeriod(1);
		reset();
	}
	
	timer::~timer() { }

	void timer::reset()
	{
		_start = std::chrono::high_resolution_clock::now();
	}

	

	double timer::getElapsedTimeInMicroSec()
	{
		return getElapsedTime<std::micro>();
	}
	

	double timer::getElapsedTimeInMilliSec()
	{
		
		return getElapsedTime<std::milli>();
	}

	double timer::getElapsedTimeInSec()
	{
		return getElapsedTime();
	}

}

