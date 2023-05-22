#pragma once

#include "debug.h"
#include <memory>
#include <shlobj_core.h>
#include <Shlwapi.h>
#include <string>

enum LogLevel
{
	// Spare Space  = 1,		    /* Performance Logging */
	LogPerf = 1 << 1,		    /* Performance Logging */
	LogDebug = 1 << 2,			/* Developer/Debug Info */
	LogInfo = 1 << 3,			/* Info Verbose High */
	LogWarning = 1 << 4,			/* Warnings  */
	LogError = 1 << 5,			/* Fatal Errors Only */
	LogSystem = 1 << 6,			/* System Logging (state, progress info) */
	LevelMask = 0x00FF,		    /* Mask Level  0-7*/


	// Sources
	LogGame = 1 << 8,		/* Game Logging */
	LogRuntime = 1 << 9,		/* Runtime Logging */
	LogPlugin = 1 << 10,		/* Plugin Logging */
	AllSources = LogGame | LogRuntime | LogPlugin,            /* AllSources and LogDebug */
	SourceMask = 0xFF00,		/* Game Logging */
	DefaultLogLevel = (AllSources | LogDebug)
};

class LogCallback
{
public:
	static const char* _logFileName;

	std::string		_logPath;
	static int		_logLevel;
	static FILE*		_hFileHandle;
	static CRITICAL_SECTION _hSection;
	static	bool		_initialized;

	LogCallback(int level, const char* logPath) : _logPath(logPath)
    {
		InitializeCriticalSection(&_hSection);
		_logLevel = level;
        _initialized = true;
	}

	~LogCallback()
    {
		EnterCriticalSection(&_hSection);

		if(_hFileHandle != nullptr)
		{
			fclose(_hFileHandle);
			_hFileHandle = nullptr;
		}

		_initialized = false;
		LeaveCriticalSection(&_hSection);
		DeleteCriticalSection(&_hSection);
	}

	void SetLogLevel(int mask)
	{
		_logLevel = mask;
	}

	int LogLevel()
	{
		return _logLevel;
	}

public:
	static LogCallback* Create(int level, const char* logPath)
	{
		return new LogCallback(level, logPath);
	}
	
	#define MegaBytes(a) a*(1<<20)

	static uint64_t MaxFileSize() { return MegaBytes(20); }
	static uint64_t MaxLogFiles() { return 100; }
	virtual void Log(int log_level, const char* source, const char* message)
	{
		Log(_logPath.c_str(), _logFileName, log_level, (log_level & LogGame ? "Game" : "Runtime"), source, message);
	}
    template<typename ...T>
	static void SLogFormat(const char* folder, int log_level,const char* source, const char* fmt, T... args)
	{
        const int LOG_BUFFER_SIZE = 4096;
        std::unique_ptr<char[]> log_buffer = std::make_unique<char[]>(LOG_BUFFER_SIZE);
		if(std::snprintf(log_buffer.get(), LOG_BUFFER_SIZE, fmt, args ...) > 0)
         	Log(folder, _logFileName, log_level,"Host",source, log_buffer.get());
	}

	static void SLog(const char* folder, int log_level, const char* source, const char* message)
	{
		Log(folder, _logFileName, log_level, "Host", source, message);
	}

	static void Log(const char* folder, const char* filename,
		int log_level, const char* source,
		const char* context_source, const char* message)
	{

		int target_loglevel = (log_level & LevelMask);
		int loglevel = (_logLevel & LevelMask);

		if (loglevel > target_loglevel)
			return;

		int log_source = log_level & SourceMask;

		if (log_source != 0)
		{
			if ((_logLevel & log_source) != (log_source))
				return;
		}

		const char* slevel = "";
		switch (loglevel)
		{
		case LogDebug:
			slevel = "DEBUG";
			break;
		case LogInfo:
			slevel = "INFO";
			break;
		case LogWarning:
			slevel = "WARNING";
			break;
		case LogError:
			slevel = "ERROR";
			break;
		case LogSystem:
			slevel = "SYSTEM";
			break;
		}

		SYSTEMTIME lt;
		GetLocalTime(&lt);
		char time_buffer[256];
		std::snprintf(time_buffer, sizeof(time_buffer), "%d-%d-%d %d:%d:%d.%3.3d",
			lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);

		std::string s;
		s.reserve(1024);
		s.append("[").append(time_buffer).append("] ");
		s.append(context_source).append("(").append(source).append(") [").append(std::to_string(GetCurrentThreadId())) += "] ";
		s.append(slevel).append(": ").append(message) += '\n';

		WriteLog(folder, filename, s.c_str(), s.length());
	}

	static void TruncateFile(const char* folder, const char* filename)
	{
		char oldest_logfile[MAX_PATH];
		int highest_index = 0;
		uint32_t count = LogFileCount(filename, oldest_logfile, &highest_index);
		
		char newFileName[MAX_PATH+1];
		sprintf(newFileName, "%s.%d", filename, highest_index+1);
		::MoveFileA(filename, newFileName);
		
		if(count > MaxLogFiles() && strlen(oldest_logfile) > 0)
		{
			char fullpath_oldest[MAX_PATH + 1];
			::PathCombineA(fullpath_oldest, folder, oldest_logfile);
			// delete oldest file
			::DeleteFileA(fullpath_oldest);
		}
	}

	static uint32_t LogFileCount(const char* filename, char* oldest_file = nullptr, int* highestIndex = nullptr)
	{
		uint32_t index = 0;
		if(highestIndex != nullptr) *highestIndex = 0;
		WIN32_FIND_DATAA ffd;
		char folderFind[MAX_PATH];
		sprintf_s(folderFind, "%s.*", filename);

		HANDLE hFind = INVALID_HANDLE_VALUE;
		
		hFind = ::FindFirstFileA(folderFind, &ffd);
		FILETIME ftmin;
#undef min
#undef max
		ftmin.dwHighDateTime = std::numeric_limits<DWORD>::max();
		ftmin.dwLowDateTime = std::numeric_limits<DWORD>::max();

		if (INVALID_HANDLE_VALUE == hFind)
		{
			return false;
		}
		do
		{
			if (((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == false) )
			{
				++index;
				char* ext = ::PathFindExtensionA(ffd.cFileName);
				if(ext != nullptr) ext++;
				int ni = strtoul(ext, nullptr,10);
				if(highestIndex != nullptr && *highestIndex < ni) *highestIndex = ni;
				if (oldest_file != nullptr)
				{
					if (ftmin.dwHighDateTime > ffd.ftLastWriteTime.dwHighDateTime)
					{
					
						strcpy(oldest_file, ffd.cFileName);
						ftmin = ffd.ftLastWriteTime;
					}
					else if (ftmin.dwHighDateTime == ffd.ftLastWriteTime.dwHighDateTime && ftmin.dwLowDateTime > ffd.ftLastWriteTime.dwLowDateTime)
					{
						strcpy(oldest_file, ffd.cFileName);
						ftmin = ffd.ftLastWriteTime;
					}
				}
			}
		} while (::FindNextFileA(hFind, &ffd) != 0);
		::FindClose(hFind);
		return index;
	}

	static void WriteLog(const char* folder, const char* filename, const char* msg, const size_t length)
	{
		if (folder == nullptr || strlen(folder) == 0 || filename == nullptr || strlen(filename) == 0)
		{
			::OutputDebugStringA(msg);
		}
		else
		{
			::SHCreateDirectoryExA(NULL, folder, NULL);

			char fullpath[MAX_PATH+1];
			::PathCombineA(fullpath, folder, filename);
			
			if(_initialized)
			{
				::EnterCriticalSection(&_hSection);
			}

			if (_hFileHandle == nullptr)
			{
				_hFileHandle = fopen(fullpath, "a");
			}

			{
				bool truncate_file = false;

				fwrite(msg, 1, length, _hFileHandle);
				
                if (ftell(_hFileHandle) > MaxFileSize())
				{
					truncate_file = true;
				}
				
                fflush(_hFileHandle);
				
                if (truncate_file)
				{
					fclose(_hFileHandle);
					TruncateFile(folder,fullpath);
					_hFileHandle = fopen(fullpath,"a");
				}
			}
			
			if(!_initialized)
			{
				fclose(_hFileHandle);
				_hFileHandle = nullptr;
			}
			else
			{
				::LeaveCriticalSection(&_hSection);
			}
		}
	}
};
