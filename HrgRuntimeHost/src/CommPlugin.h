#pragma once

#include "LogCallback.h"
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

class CommPlugin
{
public:
    CommPlugin(LogCallback* _pLogCallback);
    ~CommPlugin();
    void Start();
    void Stop();
    void Update(double elapsedTime);

private:
    NamedPipeClientTransport* _pPlatformClientTransport;
    Channel* _pPlatformClientChannel;
    GameCallbacks* _pGameCallbacks;
    PresentationCallbacks* _pPresentationCallbacks;

    ServiceCallbacks* _pServiceCallbacks;
    RuntimeServiceCallback* _pRuntimeServiceCallbacks;
    RuntimePresentationServiceCallback* _pRuntimePresentationServiceCallbacks;
    NamedPipeServerTransport* _pPlatformServerTransport;
    Server* _pPlatformServer;

    NamedPipeClientTransport* _pGameClientTransport;
    Channel* _pGameClientChannel;
    RuntimeCallbacks* _pRuntimeCallbacks;
    RuntimePresentationCallbacks* _pRuntimePresentationCallbacks;

    ServiceCallbacks* _pGamesideServiceCallbacks;
    GameServiceCallback* _pGameServiceCallbacks;
    PresentationServiceCallback* _pPresentationServiceCallbacks;
    NamedPipeServerTransport* _pGameServerTransport;
    Server* _pGameServer;
};
