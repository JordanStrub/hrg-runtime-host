#include "CommPlugin.h"
#include "Server.h"
#include "Logger.h"

using namespace Aristocrat::Snapp;

static LogCallback* pLog = nullptr;
CommPlugin* CommPlugin::commPlugin = nullptr;

const wchar_t* MonacoServerPipeName = L"MonacoRuntimeSnapp";
const wchar_t* MonacoClientPipeName = L"MonacoPlatformSnapp";
const wchar_t* HostServerPipeName = L"HostRuntimeSnapp";
const wchar_t* HostClientPipeName = L"HostGameSnapp";

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

CommPlugin::CommPlugin(LogCallback* pLogCallback, Runtime* pRuntime)
    : _pRuntime(pRuntime)
{
    commPlugin = this;
    pLog = pLogCallback;
    SetExternalLogger(StaticLog);

    // Platform-side Server
    _pPlatformServiceCallbacks = new ServiceCallbacks();
    _pRuntimeServiceCallbacks = new RuntimeServiceCallback(pLog);
    _pRuntimePresentationServiceCallbacks = new RuntimePresentationServiceCallback(pLog);
    _pPlatformServiceCallbacks->AddCallback(*_pRuntimeServiceCallbacks);
    _pPlatformServiceCallbacks->AddCallback(*_pRuntimePresentationServiceCallbacks);

    _pPlatformServerTransport = new NamedPipeServerTransport(MonacoServerPipeName);
    _pPlatformServer = new Server(_pPlatformServerTransport, _pPlatformServiceCallbacks);
    _pPlatformServer->Start();
    pLog->Log(LogInfo, "CommPlugin", "Started platform server comms");


    // Platform-side Client
    _pPlatformClientTransport = new NamedPipeClientTransport(MonacoClientPipeName);
    _pPlatformClientChannel = new Channel(_pPlatformClientTransport);

    _pGameCallbacks = new GameCallbacks(pLog, _pPlatformClientChannel);
    _pPresentationCallbacks = new PresentationCallbacks(_pGameCallbacks, pLog, _pPlatformClientChannel);

    _pPlatformClientChannel->Connect();
    pLog->Log(LogInfo, "CommPlugin", "Started platform client comms");


    // Game-side Client, later we'll connect.
    _pGamesideClientTransport = new NamedPipeClientTransport(HostClientPipeName);
    _pGamesideClientChannel = new Channel(_pGamesideClientTransport);

    _pRuntimeCallbacks = new RuntimeCallbacks(pLog, _pGamesideClientChannel);
    _pRuntimePresentationCallbacks = new RuntimePresentationCallbacks(_pRuntimeCallbacks, pLog, _pGamesideClientChannel);


    // Game-side Server
    _pGamesideServiceCallbacks = new ServiceCallbacks();
    _pGameServiceCallbacks = new GameServiceCallback(pLog, _pRuntime, _pRuntimeCallbacks, _pGamesideClientChannel);
    _pPresentationServiceCallbacks = new PresentationServiceCallback(pLog);
    _pGamesideServiceCallbacks->AddCallback(*_pGameServiceCallbacks);
    _pGamesideServiceCallbacks->AddCallback(*_pPresentationServiceCallbacks);

    _pGamesideServerTransport = new NamedPipeServerTransport(HostServerPipeName);
    _pGamesideServer = new Server(_pGamesideServerTransport, _pGamesideServiceCallbacks);
    _pGamesideServer->Start();
    pLog->Log(LogInfo, "CommPlugin", "Started game-side server comms");
}

CommPlugin::~CommPlugin()
{
    delete _pRuntimePresentationCallbacks;
    delete _pRuntimeCallbacks;
    delete _pGamesideClientChannel;
    delete _pGamesideClientTransport;

    _pGamesideServer->Stop();
    delete _pGamesideServer;
    delete _pGamesideServerTransport;
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
    delete _pPlatformServiceCallbacks;
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
    _pGameCallbacks->set_communication_open(_pPlatformClientChannel->IsConnected());
    _pRuntimeCallbacks->set_communication_open(_pGamesideClientChannel->IsConnected());
}
