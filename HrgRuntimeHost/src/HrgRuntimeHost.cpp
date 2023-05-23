//
//	HrgRuntimeHost.cpp : This file contains the 'main' function. Program execution begins and ends here.
//

#include "LogCallback.h"
#include "optionparser.h"
#include "timer.h"
#include "winutil.h"
#include "debug.h"
#include "simplewindow.h"

#include "Runtime.h"
#include "CommPlugin.h"

#include <functional>
#include <Shlwapi.h>
#include <signal.h>
#include <synchapi.h>
#include <minwindef.h>
#include <winnt.h>

#ifdef _DEBUG
#define Build_Configuration  "Debug"
#else 
#define Build_Configuration  "Release"
#endif

FILE* LogCallback::_hFileHandle = NULL;
const char* LogCallback::_logFileName = "Runtime.log";
CRITICAL_SECTION LogCallback::_hSection = { 0 };
int LogCallback::_logLevel = DefaultLogLevel;
bool LogCallback::_initialized = false;
const char* APPLICATION_NAME = "HrgRuntimeHost";
const char* CONFIG = "Config";
const char* MARKET = "Market";
const char* MARKET_CONFIGURATION_NAME = "/Runtime/Market/ConfigurationName";

HANDLE _hRunOnceMutex = NULL;

static char __log_folder[MAX_PATH] = { 0 };

enum optionIndex {
	UNKNOWN, 
	GAMEMODULE,
	VARIATION,
	DENOMINATION,
	LOGPATH,
	// Below are normal from Monaco but unused here
	DISPLAY0, DISPLAY1, DISPLAY2, DISPLAY3,
	IPC_PLUGIN,
	JURISDICTION,
	FPSLIMIT,
	CDSPLUGINS,
};

using namespace aristocrat;

const option::Descriptor usage[] =
{
	{ optionIndex::UNKNOWN,  "", "", option::Arg::None, "USAGE: example [options]\n\n"
	"Options:" },
	{ GAMEMODULE,	"g", "game", option::Arg::String /*| option::Arg::Required */, "  --game, -g \tgame module to load." },
	{ VARIATION,	"v", "variation", option::Arg::String,  "  --variation, -v \tset Game Variation." },
	{ DENOMINATION, "de", "denomination", option::Arg::Numeric, "--denomination, -de \tdenomination"},
	{ LOGPATH,		"lg", "log", option::Arg::String,		"  --log, -lg \tset the logging path (relative to game or absolute)." },
	{ DISPLAY0,		"d0", "display0", option::Arg::Numeric,	"  --display0, -d0 \tset Window Display 0 handle (unused)." },
	{ DISPLAY1,		"d1", "display1", option::Arg::Numeric, "  --display1, -d1 \tset Window Display 1 handle (unused)." },
	{ DISPLAY2,		"d2", "display2", option::Arg::Numeric, "  --display2, -d2 \tset Window Display 2 handle (unused)." },
	{ DISPLAY3,		"d3", "display3", option::Arg::Numeric, "  --display3, -d3 \tset Window Display 3 handle (unused)." },
	{ IPC_PLUGIN,	"w", "wcf", option::Arg::None,			"  --wcf, -w \tEnable Snapp Plugin (built into host).  We have to keep this command line option because we can't make new command line option requirements of the Platform (for backward compatibility). (unused)" },
	{ JURISDICTION,	"j", "jurisdiction", option::Arg::String,		"  --jurisdiction, -j \tset the jurisdiction preset configuration. (unused)" },
	{ FPSLIMIT,		"f", "fps", option::Arg::Numeric,		"  --fps, -f \toverride fps limit (0 = unlimited). (unused)" },
	{ CDSPLUGINS,	"cds", "cds", option::Arg::None,  " --cds  \tauto load required plugins for CDS games (unused)"},
	{ optionIndex::UNKNOWN,  "", "", option::Arg::None,					"\nExamples:\n"
	"  example -g ../Games/TemplateGame/TemplateGame.dll\n"
	"  example \n" },
	{ 0, 0, 0, option::Arg::None, 0 }
};

std::function<int(int, char*, int*)> __report_hook_func = nullptr;

int __ReportHook(int rtype, char* message, int* returnValue)
{
	if (__report_hook_func)
	{
		return __report_hook_func(rtype, message, returnValue);
	}
	return 0;
}

int MemoryCheckedMain(HINSTANCE hInstance, option::Options& opts)
{
	//int			logLevel = 0; // None
	const char* gameModulePath = nullptr;
	const char* gameModuleName = nullptr;
	std::string gameVariation;
	int gameDenomination = 1;

	std::string appName(APPLICATION_NAME);

	if (opts[LOGPATH])
	{
		strcpy_s(__log_folder, opts.GetValue(LOGPATH));
	}

	if (opts[GAMEMODULE])
	{
		gameModulePath = opts.GetValue(GAMEMODULE);
		gameModuleName = PathFindFileNameA(gameModulePath);

		appName.push_back('-');
		appName.append(gameModuleName);
		appName = appName.substr(0, appName.rfind('.')); //PathRemoveExtension
	}

	_hRunOnceMutex = OpenMutexA(MUTEX_ALL_ACCESS, 0, "RuntimeHostProcess");

	LogCallback::SLog(__log_folder, LogInfo, "Host", "");
	LogCallback::SLog(__log_folder, LogInfo, "Host",
		"-------------------------------------------------------------------------------------------------");
	LogCallback::SLog(__log_folder, LogInfo, "Host", "");

	LogCallback::SLogFormat(__log_folder, LogInfo, "Host",
		"Host Process Started with Arguments: %s ",
		::GetCommandLineA(), _hRunOnceMutex != NULL ? "Process Already Running" : "Single Runtime Process");

	if (!_hRunOnceMutex)
	{
		_hRunOnceMutex = CreateMutexA(0, 0, "RuntimeHostProcess");
	}

	if (opts[VARIATION])
	{
		const char* l_gameVariation = opts.GetValue(VARIATION);
		if (l_gameVariation)
		{
			gameVariation = l_gameVariation;
		}
		LogCallback::SLogFormat(__log_folder, LogInfo, "Args", "variation %s", gameVariation.c_str());
	}
	else
	{
		// no default
		gameVariation = "99";
	}

	if (opts[DENOMINATION])
	{
		opts.GetArgument(DENOMINATION, gameDenomination);
		LogCallback::SLogFormat(__log_folder, LogInfo, "Args", "Denomination %lu", static_cast<unsigned long>(gameDenomination));
	}

	LogCallback::SLog(__log_folder, LogInfo, "Args", "Using SNAPP");

	// Uncomment following line if you want process to pause until a debugger is attached.
	// WaitForDebugger();

	const char* curDir = SwitchToExecutablesDirectory(hInstance);
	LogCallback::SLogFormat(__log_folder, LogInfo, "Init", "Executable directory %s", curDir);

	if (gameModulePath)
	{
		LogCallback::SLogFormat(__log_folder, LogInfo, "Args", "Module path %s", gameModulePath);
		char gameModuleFolder[MAX_PATH] = { '\0' };
		memcpy_s(gameModuleFolder, MAX_PATH, gameModulePath, strlen(gameModulePath) + 1);
		PathRemoveFileSpecA(gameModuleFolder);
		LogCallback::SLogFormat(__log_folder, LogInfo, "Init", "Working directory %s", gameModuleFolder);
		::SetCurrentDirectoryA(gameModuleFolder);
	}
	else
	{
		LogCallback::SLogFormat(__log_folder, LogError, "Init", "Game module path is Null.");
		return 1;
	}

    {
		auto logger = new LogCallback(DefaultLogLevel, __log_folder);
		Runtime* pRuntime = new Runtime(logger);
		CommPlugin* pCommPlugin = new CommPlugin(logger);

		// pass information to GDKRuntime
		if (!gameVariation.empty())
		{
			if (!pRuntime->SetParameter("/Runtime/Variation/SelectedID", gameVariation.c_str(), CONFIG))
			{
				LogCallback::SLogFormat(__log_folder, LogInfo, "Args", "Variation ID not accepted %s", gameVariation.c_str());
			}
		}

		if (gameDenomination > 0)
		{
			char denomStr[MAX_PATH];
			sprintf(denomStr, "%d", gameDenomination);
			pRuntime->SetParameter("/Runtime/Denomination", denomStr, CONFIG);
		}

		LogCallback::SLog(__log_folder, LogInfo, "Init", "Configured Runtime");

		//pCommPlugin->Start();
		//LogCallback::SLog(__log_folder, LogInfo, "Init", "Started SNAPP host.");

		LogCallback::SLogFormat(__log_folder, LogInfo, "Init", "Loading Game Module %s.", gameModulePath);
		/* try and load a game module from path provided */

		if (pRuntime->LoadFromModule(gameModulePath))
		{
			LogCallback::SLog(__log_folder, LogInfo, "Init", "Game Module loaded successfully.");
			// game ignores index provided, it just takes the order of the list given.. (sort needed in AddDisplay)
			/* Pre-loading configurations */
			pRuntime->Configure();
			LogCallback::SLog(__log_folder, LogInfo, "Init", "Runtime Configuration Complete.");

			pCommPlugin->Start();
			LogCallback::SLog(__log_folder, LogInfo, "Init", "Started SNAPP host.");

			if (pRuntime->Initialize())/* Initializes game (will create instance of client and server) */
			{
				LogCallback::SLog(__log_folder, LogInfo, "Init", "Runtime Initialized.");

				LogCallback::SLog(__log_folder, LogInfo, "Init", "Entering Main Loop.");

				try
                {
                    timer global_timer;
                    // Message Pumping (global) until WM_QUIT (fired by PostQuit)
					while (SimpleWindow::RunMessageLoopOnce())
					{
						double elapsedTime = (double)global_timer.getElapsedTimeInSec();

						try
						{
								pCommPlugin->Update(elapsedTime);
						}
						catch (std::exception& e)
						{

        						LogCallback::SLogFormat(__log_folder, LogInfo, "Exit", "exception: infer shutdown signal\n%s", e.what());
							pRuntime->Shutdown();
						}

						if (pRuntime->IsPendingShutdown())
						{
							LogCallback::SLogFormat(__log_folder, LogInfo, "Main", "Received Shutdown.");
							PostQuitMessage(0);
							break;
						}
					}
				}
				catch (std::exception& fe)
				{
					LogCallback::SLogFormat(__log_folder, LogInfo, "Exit", "Runtime Fatal Exception\n%s", fe.what());
					PostQuitMessage(0);
				}
				LogCallback::SLogFormat(__log_folder, LogInfo, "Main", "Main loop exited");
			}
		}
		else
		{
			LogCallback::SLogFormat(__log_folder, LogInfo, "Init", "Failed Loading %s", gameModulePath);
		}
	}

	return 0;
}

void on_exit()
{
	LogCallback::SLogFormat(__log_folder, LogInfo, "Exit", "Application Exited");
}

void signalHandler(int signum) {
	LogCallback::SLogFormat(__log_folder, LogInfo, "signalHandler", "signum %d", signum);
}

int main(int argc, char** argv)
{
	::DisableProcessWindowsGhosting();
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	SwitchToExecutablesDirectory(hInstance);
	signal(SIGABRT, signalHandler);
	signal(SIGTERM, signalHandler);
	option::Options opts(const_cast<option::Descriptor*>(usage));
	if (!opts.Parse(argv+1, argc))
	{
		printf("%s", opts.error_msg());
		OutputDebugStringA(opts.error_msg());
		LocalFree(argv);
		return -1;
	}

	_CrtSetReportMode(_CRT_ASSERT, 0);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportHook(__ReportHook);

	_CrtMemState s1;
	UNREFERENCED_PARAMETER(s1);
	// Memory checkpoint .
	_CrtMemCheckpoint(&s1);
	int ret = 0;
	try
	{
		ret = MemoryCheckedMain(hInstance, opts);
	}
	catch (const std::exception& e)
	{
		LogCallback::SLogFormat(__log_folder, LogInfo, "Exit", "Process exception %s.", e.what());
	}
	catch (const int i)
	{
		LogCallback::SLogFormat(__log_folder, LogInfo, "Exit", "Process exception id %d.", i);
	}
	catch (const long l)
	{
		LogCallback::SLogFormat(__log_folder, LogInfo, "Exit", "Process exception id %ld.", l);
	}
	catch (const char* p)
	{
		LogCallback::SLogFormat(__log_folder, LogInfo, "Exit", "Process exception id %s.", p);
	}
	catch (...)
	{
		LogCallback::SLogFormat(__log_folder, LogInfo, "Exit", "Process unhandled exception.");
	}

	ReleaseMutex(_hRunOnceMutex);

	LocalFree(argv);

	// dumps all memory allocations since checkpoint (s1)
	_CrtMemDumpAllObjectsSince(&s1);

	// additional statistics
	_CrtMemDumpStatistics(&s1);

	return ret;
}
