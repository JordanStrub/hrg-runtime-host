#include "CommPlugin.h"
#include "Server.h"
#include "Logger.h"

using namespace Aristocrat::Snapp;

static LogCallback* pLog = nullptr;
static CommPlugin* commPlugin = nullptr;

const wchar_t* MonacoServerPipeName = L"MonacoRuntimeSnapp";
const wchar_t* MonacoClientPipeName = L"MonacoPlatformSnapp";

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

CommPlugin::CommPlugin(LogCallback* pLogCallback)
{
    commPlugin = this;
    pLog = pLogCallback;
    SetExternalLogger(StaticLog);

    // Platform-side Server
    _pServiceCallbacks = new ServiceCallbacks();
    _pRuntimeServiceCallbacks = new RuntimeServiceCallback(pLog);
    _pRuntimePresentationServiceCallbacks = new RuntimePresentationServiceCallback(pLog);
    _pServiceCallbacks->AddCallback(*_pRuntimeServiceCallbacks);
    _pServiceCallbacks->AddCallback(*_pRuntimePresentationServiceCallbacks);

    _pPlatformServerTransport = new NamedPipeServerTransport(MonacoServerPipeName);
    _pPlatformServer = new Server(_pPlatformServerTransport, _pServiceCallbacks);
    _pPlatformServer->Start();
    pLog->Log(LogInfo, "CommPlugin", "Started server comms");


    // Platform-side Client
    _pPlatformClientTransport = new NamedPipeClientTransport(MonacoClientPipeName);
    _pPlatformClientChannel = new Channel(_pPlatformClientTransport);

    _pGameCallbacks = new GameCallbacks(pLog, _pPlatformClientChannel);
    _pPresentationCallbacks = new PresentationCallbacks(_pGameCallbacks, pLog, _pPlatformClientChannel);

    _pPlatformClientChannel->Connect();
    pLog->Log(LogInfo, "CommPlugin", "Started client comms");


    // Game-side Server

    // Game-side Client

}

CommPlugin::~CommPlugin()
{
    delete _pRuntimePresentationCallbacks;
    delete _pRuntimeCallbacks;
    delete _pGameClientChannel;
    delete _pGameClientTransport;

    _pGameServer->Stop();
    delete _pGameServer;
    delete _pGameServerTransport;
    delete _pGameServiceCallbacks;
    delete _pPresentationServiceCallbacks;
    delete _pGameServiceCallbacks;


    delete _pPresentationCallbacks;
    delete _pGameCallbacks;
    delete _pPlatformClientChannel;
    delete _pPlatformClientTransport;

    _pPlatformServer->Stop();
    delete _pPlatformServer;
    delete _pPlatformServerTransport;
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

void CommPlugin::Update(double elapsedTime)
{
    
}
