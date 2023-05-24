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
    _pRuntimeReelServiceCallbacks = new RuntimeReelServiceCallback(pLog);
    _pRuntimePresentationServiceCallbacks = new RuntimePresentationServiceCallback(pLog);
    _pServiceCallbacks->AddCallback(*_pRuntimeServiceCallbacks);
    _pServiceCallbacks->AddCallback(*_pRuntimeReelServiceCallbacks);
    _pServiceCallbacks->AddCallback(*_pRuntimePresentationServiceCallbacks);

    _pServerTransport = new NamedPipeServerTransport(MonacoServerPipeName);
    _pPlatformServer = new Server(_pServerTransport, _pServiceCallbacks);
    _pPlatformServer->Start();
    pLog->Log(LogInfo, "CommPlugin", "Started server comms");


    // Platform-side Client
    _pClientTransport = new NamedPipeClientTransport(MonacoClientPipeName);
    _pClientChannel = new Channel(_pClientTransport);

    _pGameCallbacks = new GameCallbacks(pLog, _pClientChannel);
    _pReelCallbacks = new ReelCallbacks(_pGameCallbacks, pLog, _pClientChannel);
    _pPresentationCallbacks = new PresentationCallbacks(_pGameCallbacks, pLog, _pClientChannel);

    _pClientChannel->Connect();
    pLog->Log(LogInfo, "CommPlugin", "Started client comms");


    // Game-side Client

    // Game-side Server

}

CommPlugin::~CommPlugin()
{
    delete _pPresentationCallbacks;
    delete _pReelCallbacks;
    delete _pGameCallbacks;
    delete _pClientChannel;
    delete _pClientTransport;

    _pPlatformServer->Stop();
    delete _pPlatformServer;
    delete _pServerTransport;
    delete _pRuntimePresentationServiceCallbacks;
    delete _pRuntimeReelServiceCallbacks;
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
