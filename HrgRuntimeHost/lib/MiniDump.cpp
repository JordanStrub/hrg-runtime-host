#define _MINIDUMPIMPL_
#define _CRT_NON_CONFORMING_SWPRINTF
#include <tchar.h>
#include <stdio.h> 
#include <windows.h>
#include <assert.h>
#include <string>
#include <crtdbg.h>
#define _IMAGEHLP_

#include "minidump.h"
#include <Shlobj.h>
#pragma warning( push )
#pragma warning(disable:4091)
#include <dbghelp.h>
#pragma warning( pop ) 
#include <sys/timeb.h>
#include <time.h>
#include <locale.h>
#include <iomanip>
#include <tlhelp32.h>
#include "symlookup.h"
#include <intrin.h>

typedef LONG NTSTATUS;

/*
 * Exception code used in _invalid_parameter
 */

#ifndef STATUS_INVALID_PARAMETER
#define STATUS_INVALID_PARAMETER         ((NTSTATUS)0xC000000DL)
#endif  /* STATUS_INVALID_PARAMETER */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void * _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)

#ifdef _X86_
void * _AddressOfReturnAddress(void);
#pragma intrinsic(_AddressOfReturnAddress)
#endif  /* _X86_ */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);


void DoSuspendThread(DWORD targetProcessId, DWORD targetThreadId);
using namespace std;

#undef _stprintf
#ifdef _UNICODE	// fixes VS 2008 SWPRINTF CONFORMING macro bug
	
	#define _stprintf swprintf 
#else
	#define _stprintf sprintf_s
#endif

const char* MiniDumper::_szAppName;
DWORD64* MiniDumper::_vAddrStack;
std::basic_string<char> MiniDumper::_szLogFile;
std::basic_string<char> MiniDumper::_szDumpDirectory;
UINT MiniDumper::_max_callstack;
LONG MiniDumper::_retvalOnSucess;
bool MiniDumper::_reuseOutputFile;

MiniDumper::MiniDumper()
{
}


MiniDumper::MiniDumper( const char* szAppName, const char* dumpdir,
    const char* logfile, unsigned short max_callstack, bool reuseOutputFile)
{
	Initialize(szAppName, dumpdir, logfile, max_callstack, EXCEPTION_EXECUTE_HANDLER, reuseOutputFile);
}

void MiniDumper::Initialize(const char* szAppName, const char* dumpdir,
    const char* logfile, unsigned short max_callstack, LONG retOnSuccess, bool reuseOutputFile)
{
	// if this assert fires then you have two instances of MiniDumper
	// which is not allowed
	assert( _szAppName==NULL );
	_max_callstack = max_callstack;
	static std::basic_string<char> appName(szAppName ? szAppName : "Application");
	_szAppName = appName.data();
    _reuseOutputFile = reuseOutputFile;
	if(dumpdir)
		_szDumpDirectory = basic_string<char>(dumpdir);
	if (logfile)
		_szLogFile = basic_string<char>(logfile);
	if(max_callstack > 0 && max_callstack < 1024)
	 {
		static std::vector<DWORD64> store;
		store.resize(sizeof(DWORD64)*max_callstack);
		_vAddrStack = store.data();
	}
	else
	{
		_vAddrStack = nullptr;
	}
	_retvalOnSucess = retOnSuccess;
	
    ::SetUnhandledExceptionFilter( TopLevelFilter );
    _set_invalid_parameter_handler(InvalidParameterHandler);
}


void MiniDumper::ShowSymbolInfo(DWORD64 ModBase, bool ignoreWindowsBinaries)
{
	// Get module information 

	IMAGEHLP_MODULE64 ModuleInfo;

	memset(&ModuleInfo, 0, sizeof(ModuleInfo));

	ModuleInfo.SizeOfStruct = sizeof(ModuleInfo);

	BOOL bRet = ::SymGetModuleInfo64(GetCurrentProcess(), ModBase, &ModuleInfo);

	if (!bRet)
	{
		LogFormat("MiniDump", "Error: SymGetModuleInfo64() failed. Error code: %u ", ::GetLastError());
		return;
	}
	if (ignoreWindowsBinaries && 
		(strstr(ModuleInfo.ImageName, "C:\\Windows") != 0  ||
		 strstr(ModuleInfo.ImageName, "C:\\Program") != 0))
		return;

	/*if (strlen(ModuleInfo.ImageName) > 0)
	{
		LogFormat("MiniDump", "Image name: %s ", ModuleInfo.ImageName);
	}*/

	/*if (strlen(ModuleInfo.LoadedImageName) > 0)
	{
		LogFormat("MiniDump", "Loaded image name: %s ", ModuleInfo.LoadedImageName);
	}*/

	// Loaded PDB name 

	

	switch (ModuleInfo.SymType)
	{
	case SymNone:
		LogFormat("MiniDump", "No symbols available for the module %s", ModuleInfo.ImageName);
		break;

	case SymExport:
		LogFormat("MiniDump", "No symbols available for the module %s, generated from DLL export table", ModuleInfo.ImageName);
		break;

	case SymCoff:
		LogFormat("MiniDump", "Loaded symbols: COFF");
		break;

	case SymCv:
		LogFormat("MiniDump", "Loaded symbols: CodeView");
		break;

	case SymSym:
		LogFormat("MiniDump", "Loaded symbols: SYM");
		break;

	case SymVirtual:
		LogFormat("MiniDump", "Loaded symbols: Virtual");
		break;

	case SymPdb:
		if (strlen(ModuleInfo.LoadedPdbName) > 0)
		{
			LogFormat("MiniDump", "PDB file name: %s for module %s", ModuleInfo.LoadedPdbName, ModuleInfo.ImageName);
		}
		break;

	case SymDia:
		LogFormat("MiniDump", "Loaded symbols: DIA");
		break;

	case SymDeferred:
		LogFormat("MiniDump", "Loaded symbols: Deferred"); // not actually loaded 
		break;

	default:
		LogFormat("MiniDump", "Loaded symbols: Unknown format.");
		break;
	}

	if (ModuleInfo.PdbUnmatched || ModuleInfo.DbgUnmatched)
	{
		LogFormat("MiniDump", "PDB file not matching module %s", ModuleInfo.ImageName);
	}

	// LogFormat("MiniDump", "Line numbers: %s, Global symbols: %s, Type information: %s, Source indexing : %s, Public symbols: %s", ModuleInfo.LineNumbers ? ("Available") : ("Not available"), ModuleInfo.GlobalSymbols ? ("Available") : ("Not available"), ModuleInfo.TypeInfo ? ("Available") : ("Not available"), ModuleInfo.SourceIndexed ? ("Yes") : ("No"), ModuleInfo.Publics ? ("Available") : ("Not available"));

}

LONG MiniDumper::TopLevelFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
//	HWND hParent = NULL;						// find a better value for your app

//TODO: add crashhandler stackwalker to get the debug stack (add to log4cxx)

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char szDbgHelpPath[_MAX_PATH];
	
	DoSuspendThread(GetCurrentProcessId(), GetCurrentThreadId());

	if (GetModuleFileNameA( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = strrchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			strcpy( pSlash+1, "DBGHELP.DLL");
			hDll = ::LoadLibraryA( szDbgHelpPath );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibraryA( "DBGHELP.DLL");
	}

	LPCSTR szResult = NULL;
	
	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char *szDumpPath = 0;			// Just for use with the "old" functions...
			char szScratch [_MAX_PATH*4];
			std::basic_string<char> sPathAndFileName;
			// Work out a good place for the dump file - we will first 
			// try to use the directory from where the application
			// was called = the "working directory".
			if(_szDumpDirectory.empty())
			{
				long numCharsNecessary = GetCurrentDirectoryA(0, NULL);
				bool directoryPathOk = (numCharsNecessary > 0);
				if (!directoryPathOk)	// Something is wrong with the directory string...
				{				
					// Set a default directory for the file.
					strcpy( szDumpPath, ".\\" );
				}
				else					// The directory string is Ok...
				{
					// Allocate space for the directory string plus
					// the terminating null character:
					szDumpPath = (char*)malloc(sizeof(char)*(numCharsNecessary+1));

					// Store the current directory string (not including
					// the filename):
					GetCurrentDirectoryA(numCharsNecessary+1, szDumpPath);
				}

				// At this point, szDumpPath contains a valid directory string.
				sPathAndFileName = szDumpPath;	// -just to make things easier...!
				free(szDumpPath);	
			}
			else
			{
				sPathAndFileName = _szDumpDirectory;
			}

			
			
			// If the last character in the directory string is not
			// a backslash, then add one:
			if (sPathAndFileName[sPathAndFileName.length()-1] != '\\')
				sPathAndFileName += "\\";

			// Append filename and extension:
			sPathAndFileName += _szAppName;
            if (!_reuseOutputFile)
            {
                sPathAndFileName += "-";
                char datebuf[255];
                GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "yyyy'.'MM'.'dd", datebuf, 255);
                sPathAndFileName += datebuf;

                sPathAndFileName += ".";
                char timebuf[255];
                GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "HH'.'mm'.'ss", timebuf, 255);
                sPathAndFileName += timebuf;
            }
 
            sPathAndFileName += ".dmp";

			Log("MiniDump", " ");
			Log("MiniDump", "Crash: Writing minidump");
			LogFormat("MiniDump", "Threadid: %d", GetCurrentThreadId());
			

			{
				// create the file
				HANDLE hFile = ::CreateFileA( sPathAndFileName.c_str(),
                    GENERIC_WRITE, FILE_SHARE_WRITE, 
					NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;
					uint32_t stack_count = 0;
					
                    if(!_szLogFile.empty())
					{
						Log("MiniDump", "Loading Lookup symbol tables");
						aristocrat::SymbolLookup* pLookup = aristocrat::SymbolLookup::Create();
						for (auto m : pLookup->Modules())
						{
							ShowSymbolInfo(m);
						}
						if(pLookup == nullptr)
						{
							Log("MiniDump", "Failed loading Symbol lookups");
						}
						if(pLookup != nullptr )
						{
							stack_count = pLookup->StackWalkInternal(pExceptionInfo->ContextRecord,
                                _vAddrStack, _max_callstack);
							LogFormat("MiniDump", "Callstack (%d lines):", stack_count);
							
                            for(uint32_t i = 0; i < stack_count; ++i)
							{
								const char* str = pLookup->Lookup(_vAddrStack[i]);
								
                                if(strlen(str) > 0)
								{
									Log("MiniDump", str);
								}
								else
								{
									Log("MiniDump","~~--[missing symbol information]--~~");
								}
							}
						}
						pLookup->Release();
					}

					// write the dump
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, 
						&ExInfo, NULL, NULL );

					if (bOK)
					{
                        snprintf( szScratch,  1024, "Saved dump file to '%s'", sPathAndFileName.c_str());
						szResult = szScratch;
						retval = _retvalOnSucess;
					}
					else
					{
                        snprintf( szScratch, 1024, "Failed to save dump file to '%s' (error %d)",
							sPathAndFileName.c_str(), GetLastError());
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
                    snprintf( szScratch, 1024, "Failed to create dump file '%s' (error %d)",
						sPathAndFileName.c_str(), GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}

	/*if (szResult)
		::MessageBoxA( NULL, szResult, _szAppName, MB_OK );*/

	return retval;
}

void MiniDumper::LogFormat(const char* source, const char* fmt, ...)
{
	const int LOG_BUFFER_SIZE = 4096;
	char log_buffer[LOG_BUFFER_SIZE];

	va_list argptr;
	va_start(argptr, fmt);
	
    if (-1 != (vsnprintf(log_buffer, LOG_BUFFER_SIZE, fmt, argptr)))
	{
		Log(source, log_buffer);
	}
	va_end(argptr);
}

void MiniDumper::Log(const char* source, const char* message)
{
    SYSTEMTIME lt;
    GetLocalTime(&lt);
    char time_buffer[256];
    std::snprintf(time_buffer, sizeof(time_buffer), "%d-%d-%d %d:%d:%d.%3.3d",
        lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
    
    std::stringstream ss;
    ss << "[" << time_buffer << "] " 
       << source << ": " << message << std::endl;
    
    auto str(ss.str());
    WriteLog(str.c_str(), str.length());
}

void MiniDumper::WriteLog(const char* msg, const size_t length)
{
	if (_szDumpDirectory.empty() || _szLogFile.empty())
	{
		::OutputDebugStringA(msg);
	}
	else
	{
		::SHCreateDirectoryExA(NULL, _szDumpDirectory.c_str(), NULL);

		char fullpath[MAX_PATH];

		::PathCombineA(fullpath, _szDumpDirectory.c_str(), _szLogFile.c_str());

		FILE* f = fopen(fullpath, "a");
		fwrite(msg, 1, length, f);
		fclose(f);
	}
}




void MiniDumper::InvalidParameterHandler(const wchar_t *pszExpression, const wchar_t *pszFunction, 
    const wchar_t *pszFile, unsigned int nLine, uintptr_t pReserved)
{
    /* Fake an exception for dump file. */
    EXCEPTION_RECORD ExceptionRecord;
    CONTEXT ContextRecord;
    EXCEPTION_POINTERS ExceptionPointers;

    (pszExpression);
    (pszFunction);
    (pszFile);
    (nLine);
    (pReserved);

#ifdef _X86_

    __asm {
        mov dword ptr [ContextRecord.Eax], eax
        mov dword ptr [ContextRecord.Ecx], ecx
        mov dword ptr [ContextRecord.Edx], edx
        mov dword ptr [ContextRecord.Ebx], ebx
        mov dword ptr [ContextRecord.Esi], esi
        mov dword ptr [ContextRecord.Edi], edi
        mov word ptr [ContextRecord.SegSs], ss
        mov word ptr [ContextRecord.SegCs], cs
        mov word ptr [ContextRecord.SegDs], ds
        mov word ptr [ContextRecord.SegEs], es
        mov word ptr [ContextRecord.SegFs], fs
        mov word ptr [ContextRecord.SegGs], gs
        pushfd
        pop [ContextRecord.EFlags]
    }

    ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable:4311)
    ContextRecord.Eip = (ULONG)_ReturnAddress();
    ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
    ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);

#elif defined (_IA64_) || defined (_AMD64_)

    /* Need to fill up the Context in IA64 and AMD64. */
    RtlCaptureContext(&ContextRecord);

#else  /* defined (_IA64_) || defined (_AMD64_) */

    ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif  /* defined (_IA64_) || defined (_AMD64_) */

    ZeroMemory(&ExceptionRecord, sizeof(ExceptionRecord));

    ExceptionRecord.ExceptionCode = STATUS_INVALID_PARAMETER;
    ExceptionRecord.ExceptionAddress = _AddressOfReturnAddress();

    ExceptionPointers.ExceptionRecord = &ExceptionRecord;
    ExceptionPointers.ContextRecord = &ContextRecord;
	
	
	::Sleep(20);
	// Create dump file by calling TopLevelFilter...
    TopLevelFilter(&ExceptionPointers);

	// Break into debugger if debug build.
	_CrtDbgBreak();

	// Terminate ourself.
    TerminateProcess(GetCurrentProcess(), STATUS_INVALID_PARAMETER);
}


void MiniDumper::DumpNow()
{
	InvalidParameterHandler(nullptr,nullptr,nullptr,0,0);
}


void DoSuspendThread(DWORD targetProcessId, DWORD targetThreadId)
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, targetProcessId);

	if (h != INVALID_HANDLE_VALUE)
	{
		
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te))
		{
			do
			{
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
				{
					// Suspend all threads EXCEPT the one we want to keep running
					if (te.th32ThreadID != targetThreadId && te.th32OwnerProcessID == targetProcessId)
					{
						HANDLE thread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
						if (thread != NULL)
						{

							SuspendThread(thread);
							CloseHandle(thread);
						}

					}
				}
				te.dwSize = sizeof(te);
			} while (Thread32Next(h, &te));
		}
		CloseHandle(h);
	}
	
};