#pragma once

#include "LogCallback.h"
#include "Runtime.h"
#include "Server.h"
#include "NamedPipeServerTransport.h"
#include "NamedPipeClientTransport.h"
#include "RuntimeServiceCallback.h"
#include "RuntimeCallbacks.h"
#include "RuntimePresentationServiceCallback.h"
#include "RuntimePresentationCallbacks.h"
#include "GameServiceCallback.h"
#include "GameCallbacks.h"
#include "PresentationServiceCallback.h"
#include "PresentationCallbacks.h"

using namespace Aristocrat::Snapp;
using namespace Aristocrat::GdkRuntime::v1;

class CommPlugin
{
public:
    CommPlugin(LogCallback* pLogCallback, Runtime* pRuntime);
    ~CommPlugin();
    void Start();
    void Stop();
    void CheckStatus();

    static CommPlugin* commPlugin;

private:
    Runtime* _pRuntime;

    NamedPipeClientTransport* _pPlatformClientTransport;
    Channel* _pPlatformClientChannel;
    GameCallbacks* _pPlatformGameCallbacks;
    PresentationCallbacks* _pPlatformPresentationCallbacks;

    ServiceCallbacks* _pPlatformServiceCallbacks;
    RuntimeServiceCallback* _pPlatformRuntimeServiceCallbacks;
    RuntimePresentationServiceCallback* _pPlatformRuntimePresentationServiceCallbacks;
    NamedPipeServerTransport* _pPlatformServerTransport;
    Server* _pPlatformServer;

    NamedPipeClientTransport* _pGamesideClientTransport;
    Channel* _pGamesideClientChannel;
    RuntimeCallbacks* _pGamesideRuntimeCallbacks;
    RuntimePresentationCallbacks* _pGamesideRuntimePresentationCallbacks;

    ServiceCallbacks* _pGamesideServiceCallbacks;
    GameServiceCallback* _pGamesideGameServiceCallbacks;
    PresentationServiceCallback* _pGamesidePresentationServiceCallbacks;
    NamedPipeServerTransport* _pGamesideServerTransport;
    Server* _pGamesideServer;
};
