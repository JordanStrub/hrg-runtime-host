#pragma once
#include <stdlib.h>
#include <crtdbg.h>
#include <cassert>
#include <time.h>
#include <sys/timeb.h>
#include <sstream>
#include <iomanip>
#include <ObjectArray.h>

namespace aristocrat
{
	static inline void WaitForDebugger()
	{

		while (!::IsDebuggerPresent())
			::Sleep(100);
		::DebugBreak();
	}

    template<typename ...T>
	static inline void Trace(const char* szValue, T... args)
	{
        const int size = 4096;
		char szBuffer[size] = {};
		if (-1 == (std::snprintf(szBuffer, size, szValue, args ...)))
			assert(false);
		
        const int c_nTimeLength = 30;
        		
		timeb rawtime;
		ftime(&rawtime);

		time_t	ltime;
		time(&ltime);
		struct tm* today = localtime(&ltime);
		char szTimeBuf[c_nTimeLength + 1];
		strftime(szTimeBuf, c_nTimeLength, "%Y-%m-%d %H:%M:%S", today);
        
        std::stringstream ss;
        ss << szTimeBuf << "." << std::setfill('0') << std::setw(3) << rawtime.millitm <<  " ";
		ss << szBuffer;
		
	#if defined(WIN32)
		OutputDebugStringA(ss.str().c_str());
	#else
		fprintf(stdout, ss.str().c_str());
	#endif // WIN32
	}

    template<typename ...T>
	static inline void Debug(const char* szValue, T... args)
	{
        const int size = 4096;
		char szBuffer[size] = {};
		if (-1 == (std::snprintf(szBuffer, size, szValue, args ...)))
			assert(false);

#if defined(WIN32)
		OutputDebugStringA(szBuffer);
#else
		fprintf(stdout, szBuffer);
#endif // WIN32
	}
}

#ifdef _DEBUG
	#define TRACE(a,...) aristocrat::Trace(a,##__VA_ARGS__);
	#define DEBUG(a,...) aristocrat::Debug(a,##__VA_ARGS__);
#else
#define TRACE(a,...) ((void)0)
#define DEBUG(a,...) ((void)0)
#define FORCETRACE(a,...) aristocrat::Trace(a,##__VA_ARGS__);
#define FORCEDEBUG(a,...) aristocrat::Debug(a,##__VA_ARGS__);
#endif