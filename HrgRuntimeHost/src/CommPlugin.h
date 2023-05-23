#pragma once

#include "LogCallback.h"
#include "Server.h"
#include "NamedPipeServerTransport.h"
#include "NamedPipeClientTransport.h"
#include "RuntimeServiceCallback.h"
#include "RuntimeReelServiceCallback.h"
#include "RuntimePresentationServiceCallback.h"
#include "GameCallbacks.h"
#include "ReelCallbacks.h"
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
    NamedPipeClientTransport* _pClientTransport;
    Channel* _pClientChannel;
    GameCallbacks* _pGameCallbacks;
    ReelCallbacks* _pReelCallbacks;
    PresentationCallbacks* _pPresentationCallbacks;

    ServiceCallbacks* _pServiceCallbacks;
    RuntimeServiceCallback* _pRuntimeServiceCallbacks;
    RuntimeReelServiceCallback* _pRuntimeReelServiceCallbacks;
    RuntimePresentationServiceCallback* _pRuntimePresentationServiceCallbacks;
    NamedPipeServerTransport* _pServerTransport;
    Server* _pPlatformServer;
};
