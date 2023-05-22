#include "CommPlugin.h"
#include "RuntimeServiceCallback.h"
#include "RuntimeReelServiceCallback.h"
#include "RuntimePresentationServiceCallback.h"
#include "Server.h"
#include "Logger.h"
#include "NamedPipeServerTransport.h"
#include "NamedPipeClientTransport.h"

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
    : _server(nullptr)
{
    commPlugin = this;
    pLog = pLogCallback;
    SetExternalLogger(StaticLog);
}

void CommPlugin::Start()
{

    ServiceCallbacks callbacks;
    RuntimeServiceCallback runtimeCallback(pLog);
    RuntimeReelServiceCallback runtimeReelCallback(pLog);
    RuntimePresentationServiceCallback runtimePresentationCallback(pLog);
    callbacks.AddCallback(runtimeCallback);
    callbacks.AddCallback(runtimeReelCallback);
    callbacks.AddCallback(runtimePresentationCallback);

    NamedPipeServerTransport serverTransport(MonacoServerPipeName);
    _server = new Server(&serverTransport, &callbacks);
    _server->Start();
    pLog->Log(LogInfo, "CommPlugin", "Starting comms");


}

void CommPlugin::Stop()
{
    
}

void CommPlugin::Update(double elapsedTime)
{
    
}
