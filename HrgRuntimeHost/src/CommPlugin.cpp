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

    // First build up clients, so they can be called from the opposite-side servers.
    _pPlatformClientTransport = new NamedPipeClientTransport(MonacoClientPipeName);
    _pPlatformClientChannel = new Channel(_pPlatformClientTransport);
    _pPlatformGameCallbacks = new GameCallbacks(pLog, _pPlatformClientChannel);
    _pPlatformPresentationCallbacks = new PresentationCallbacks(_pPlatformGameCallbacks, pLog, _pPlatformClientChannel);

    _pGamesideClientTransport = new NamedPipeClientTransport(HostClientPipeName);
    _pGamesideClientChannel = new Channel(_pGamesideClientTransport);
    _pGamesideRuntimeCallbacks = new RuntimeCallbacks(pLog, _pGamesideClientChannel);
    _pGamesideRuntimePresentationCallbacks = new RuntimePresentationCallbacks(_pGamesideRuntimeCallbacks, pLog, _pGamesideClientChannel);


    // Platform-side Server
    _pPlatformServiceCallbacks = new ServiceCallbacks();
    _pPlatformRuntimeServiceCallbacks = new RuntimeServiceCallback(pLog, _pGamesideRuntimeCallbacks);
    _pPlatformRuntimePresentationServiceCallbacks = new RuntimePresentationServiceCallback(pLog, _pGamesideRuntimePresentationCallbacks);
    _pPlatformServiceCallbacks->AddCallback(*_pPlatformRuntimeServiceCallbacks);
    _pPlatformServiceCallbacks->AddCallback(*_pPlatformRuntimePresentationServiceCallbacks);

    _pPlatformServerTransport = new NamedPipeServerTransport(MonacoServerPipeName);
    _pPlatformServer = new Server(_pPlatformServerTransport, _pPlatformServiceCallbacks);
    _pPlatformServer->Start();
    pLog->Log(LogInfo, "CommPlugin", "Started platform server comms");


    // Platform-side Client: Go!
    _pPlatformClientChannel->Connect();
    pLog->Log(LogInfo, "CommPlugin", "Started platform client comms");


    // Game-side Server
    _pGamesideServiceCallbacks = new ServiceCallbacks();
    _pGamesideGameServiceCallbacks = new GameServiceCallback(pLog, _pRuntime, _pGamesideRuntimeCallbacks, _pGamesideClientChannel, _pPlatformGameCallbacks);
    _pGamesidePresentationServiceCallbacks = new PresentationServiceCallback(pLog, _pPlatformPresentationCallbacks);
    _pGamesideServiceCallbacks->AddCallback(*_pGamesideGameServiceCallbacks);
    _pGamesideServiceCallbacks->AddCallback(*_pGamesidePresentationServiceCallbacks);

    _pGamesideServerTransport = new NamedPipeServerTransport(HostServerPipeName);
    _pGamesideServer = new Server(_pGamesideServerTransport, _pGamesideServiceCallbacks);
    _pGamesideServer->Start();
    pLog->Log(LogInfo, "CommPlugin", "Started game-side server comms");
}

CommPlugin::~CommPlugin()
{
    delete _pGamesideRuntimePresentationCallbacks;
    delete _pGamesideRuntimeCallbacks;
    delete _pGamesideClientChannel;
    delete _pGamesideClientTransport;

    _pGamesideServer->Stop();
    delete _pGamesideServer;
    delete _pGamesideServerTransport;
    delete _pGamesideGameServiceCallbacks;
    delete _pGamesidePresentationServiceCallbacks;
    delete _pGamesideGameServiceCallbacks;


    delete _pPlatformPresentationCallbacks;
    delete _pPlatformGameCallbacks;
    delete _pPlatformClientChannel;
    delete _pPlatformClientTransport;

    _pPlatformServer->Stop();
    delete _pPlatformServer;
    delete _pPlatformServerTransport;
    delete _pPlatformRuntimePresentationServiceCallbacks;
    delete _pPlatformRuntimeServiceCallbacks;
    delete _pPlatformServiceCallbacks;
}


void CommPlugin::Start()
{
    //::Sleep(250);
    //JoinRequest join;
    //Empty empty;
    //Status status;
    //_pPlatformGameCallbacks->Join(join, empty, status);

    //::Sleep(250);
    //RuntimeEventNotification notify;
    //notify.set_runtimeevent(RuntimeEventNotification_RuntimeEvent_RequestConfiguration);
    //_pPlatformGameCallbacks->RuntimeEvent(notify, empty, status);
}

void CommPlugin::Stop()
{
    LeaveRequest leave;
    Empty empty;
    Status status;
    _pPlatformGameCallbacks->Leave(leave, empty, status);
}

void CommPlugin::CheckStatus()
{
    _pPlatformGameCallbacks->set_communication_open(_pPlatformClientChannel->IsConnected());
    _pGamesideRuntimeCallbacks->set_communication_open(_pGamesideClientChannel->IsConnected());
}
