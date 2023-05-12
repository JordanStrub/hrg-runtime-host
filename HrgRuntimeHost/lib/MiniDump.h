#pragma once
/**********************************************************

Important Node: Only include minidump.h in main application source file.

Usage:

create an instance of MiniDump

MiniDumper minidump("application"); // the name of the application which also will be the name of the dump file

you may catch any application specific  exception, and exclude accessviolations and other runtime exception
don't use catch(...)

the minidumper catches any uncaught exceptions, and creates a minidump where the executable exists

the dump will be named application.dmp located next to the executable

*/



struct _EXCEPTION_POINTERS;

class MiniDumper
{
private:
	static DWORD64*	_vAddrStack;
	static UINT   _max_callstack;
	static const char* _szAppName;
	static std::basic_string<char> _szLogFile;
	static std::basic_string<char> _szDumpDirectory;
	static LONG _retvalOnSucess;
    static bool _reuseOutputFile;

	static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );
	static void __cdecl InvalidParameterHandler(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t);
	
    void Initialize(const char* szAppName, const char* dumpdir, 
        const char* logfile, unsigned short max_callstack, LONG retOnSuccess, bool reuseOutputFile);
	
    static void WriteLog(const char* msg, const size_t length);
	static void Log(const char* source, const char* message);
	static void LogFormat(const char* source, const char* fmt, ...);
	static void ShowSymbolInfo(DWORD64 ModBase, bool ignoreWindowsBinaries = true);
	static void DumpNow();
    
public:
	MiniDumper(const char* szAppName, const char* dumpdir = NULL, 
        const char* logfile = NULL, unsigned short max_callstack = 0, bool reuseOutputFile = true);
	
    MiniDumper();
	
};
