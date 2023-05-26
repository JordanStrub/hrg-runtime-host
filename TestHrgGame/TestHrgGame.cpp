
// TestHrgGame.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "shellapi.h"
#include "framework.h"
#include "TestHrgGame.h"

#include "CommPlugin.h"
#include "TestHrgGameDlg.h"
#include "LogCallback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

FILE* LogCallback::_hFileHandle = NULL;
const char* LogCallback::_logFileName = "Game.log";
CRITICAL_SECTION LogCallback::_hSection = { 0 };
int LogCallback::_logLevel = DefaultLogLevel;
bool LogCallback::_initialized = false;
static char __log_folder[MAX_PATH] = { 0 };
HANDLE _hRunOnceMutex = NULL;

// CTestHrgGameApp

BEGIN_MESSAGE_MAP(CTestHrgGameApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

void get_command_line_args(int* argc, char*** argv)
{
	// Get the command line arguments as wchar_t strings
	wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), argc);
	if (!wargv) { *argc = 0; *argv = NULL; return; }

	// Count the number of bytes necessary to store the UTF-8 versions of those strings
	int n = 0;
	for (int i = 0; i < *argc; i++)
		n += WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL) + 1;

	// Allocate the argv[] array + all the UTF-8 strings
	*argv = (char**)malloc((*argc + 1) * sizeof(char*) + n);
	if (!*argv) { *argc = 0; return; }

	// Convert all wargv[] --> argv[]
	char* arg = (char*)&((*argv)[*argc + 1]);
	for (int i = 0; i < *argc; i++)
	{
		(*argv)[i] = arg;
		arg += WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, arg, n, NULL, NULL) + 1;
	}
	(*argv)[*argc] = NULL;
}


// CTestHrgGameApp construction

CTestHrgGameApp::CTestHrgGameApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTestHrgGameApp object

CTestHrgGameApp theApp;


// CTestHrgGameApp initialization

BOOL CTestHrgGameApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	//AfxEnableControlContainer();

	//// Create the shell manager, in case the dialog contains
	//// any shell tree view or shell list view controls.
	//CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("TestHrgGame"));

	int argc = 0;
	char** argv;
	get_command_line_args(&argc, &argv);
	if (argc > 1)
		strcpy(__log_folder, argv[1]);
	else
		strcpy(__log_folder, ".");
	free(argv);

	_hRunOnceMutex = OpenMutexA(MUTEX_ALL_ACCESS, 0, "GameHostProcess");
	LogCallback::SLog(__log_folder, LogInfo, "Game", "");
	LogCallback::SLog(__log_folder, LogInfo, "Game",
		"-------------------------------------------------------------------------------------------------");
	LogCallback::SLog(__log_folder, LogInfo, "Game", "");
	LogCallback::SLogFormat(__log_folder, LogInfo, "Game",
		"Game Process Started with Arguments: %s ",
		::GetCommandLineA(), _hRunOnceMutex != NULL ? "Process Already Running" : "Single Game Process");
	if (!_hRunOnceMutex)
	{
		_hRunOnceMutex = CreateMutexA(0, 0, "GameHostProcess");
	}
	LogCallback::SLog(__log_folder, LogInfo, "Args", "Using SNAPP");
	auto logger = new LogCallback(DefaultLogLevel, __log_folder);
	auto pCommPlugin = new CommPlugin(logger);
	pCommPlugin->Start();
	LogCallback::SLog(__log_folder, LogInfo, "Init", "Started SNAPP host.");


	CTestHrgGameDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		LogCallback::SLog(__log_folder, LogInfo, "End", "Dialog dismissed with OK.");
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		LogCallback::SLog(__log_folder, LogInfo, "End", "Dialog dismissed with Cancel.");
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		LogCallback::SLog(__log_folder, LogInfo, "End", "Warning: dialog creation failed, so application is terminating unexpectedly.");
	}

	//// Delete the shell manager created above.
	//if (pShellManager != nullptr)
	//{
	//	delete pShellManager;
	//}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

