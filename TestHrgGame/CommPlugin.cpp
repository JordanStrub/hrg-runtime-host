#include "pch.h"
#include "CommPlugin.h"
#include "Server.h"
#include "Logger.h"

using namespace Aristocrat::Snapp;

static LogCallback* pLog = nullptr;
static CommPlugin* commPlugin = nullptr;

const wchar_t* HostServerPipeName = L"HostGameSnapp";
const wchar_t* HostClientPipeName = L"HostRuntimeSnapp";

static void StaticLog(int level, std::string message)
{
    if (pLog)
    {
        int localLevel = LogPlugin;
        switch (level)
        {
        case Debug: localLevel |= LogDebug; break;
        case Info: localLevel |= LogInfo; break;
        case Warn: localLevel |= LogWarning; break;
        case Fatal: localLevel |= LogSystem; break;
        // case Error: gives some ambiguity error, so let it be default.
        default: localLevel |= LogError; break;
        }
        pLog->Log(localLevel, "Snapp", message.c_str());
    }
}

CommPlugin::CommPlugin(LogCallback* pLogCallback, CTestHrgGameDlg* pDialog)
    : _pDialog(pDialog)
{
    commPlugin = this;
    pLog = pLogCallback;
    SetExternalLogger(StaticLog);

    // Server
    _pServiceCallbacks = new ServiceCallbacks();
    _pRuntimeServiceCallbacks = new RuntimeServiceCallback(pLog, pDialog);
    _pRuntimePresentationServiceCallbacks = new RuntimePresentationServiceCallback(pLog);
    _pServiceCallbacks->AddCallback(*_pRuntimeServiceCallbacks);
    _pServiceCallbacks->AddCallback(*_pRuntimePresentationServiceCallbacks);

    _pServerTransport = new NamedPipeServerTransport(HostServerPipeName);
    _pServer = new Server(_pServerTransport, _pServiceCallbacks);
    _pServer->Start();
    pLog->Log(LogInfo, "CommPlugin", "Started server comms");


    // Client
    _pClientTransport = new NamedPipeClientTransport(HostClientPipeName);
    pLog->Log(LogInfo, "CommPlugin", "Created client pipe transport");
    _pClientChannel = new Channel(_pClientTransport);
    pLog->Log(LogInfo, "CommPlugin", "Created client channel");

    _pGameCallbacks = new GameCallbacks(pLog, _pClientChannel);
    _pPresentationCallbacks = new PresentationCallbacks(_pGameCallbacks, pLog, _pClientChannel);
    pLog->Log(LogInfo, "CommPlugin", "Created client callbacks");

    _pClientChannel->Connect();
    pLog->Log(LogInfo, "CommPlugin", "Started client comms");
}

CommPlugin::~CommPlugin()
{
    delete _pPresentationCallbacks;
    delete _pGameCallbacks;
    delete _pClientChannel;
    delete _pClientTransport;

    _pServer->Stop();
    delete _pServer;
    delete _pServerTransport;
    delete _pRuntimePresentationServiceCallbacks;
    delete _pRuntimeServiceCallbacks;
    delete _pServiceCallbacks;
}


void CommPlugin::Start()
{
    ::Sleep(250);
    JoinRequest join;
    Empty empty;
    Status status;
    _pGameCallbacks->Join(join, empty, status);

    ::Sleep(250);
    RuntimeEventNotification notify;
    notify.set_runtimeevent(RuntimeEventNotification_RuntimeEvent_RequestConfiguration);
    _pGameCallbacks->RuntimeEvent(notify, empty, status);
}

void CommPlugin::Stop()
{
    LeaveRequest leave;
    Empty empty;
    Status status;
    _pGameCallbacks->Leave(leave, empty, status);
}

void CommPlugin::CheckStatus()
{
    
}
