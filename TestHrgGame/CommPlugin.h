#pragma once

#include "LogCallback.h"
#include "Server.h"
#include "NamedPipeServerTransport.h"
#include "NamedPipeClientTransport.h"
#include "RuntimeServiceCallback.h"
#include "RuntimePresentationServiceCallback.h"
#include "GameCallbacks.h"
#include "PresentationCallbacks.h"
#include "TestHrgGameDlg.h"

using namespace Aristocrat::Snapp;

class CommPlugin
{
public:
    CommPlugin(LogCallback* pLogCallback, CTestHrgGameDlg* pDialog);
    ~CommPlugin();
    void Start();
    void Stop();
    void CheckStatus();

private:
    CTestHrgGameDlg* _pDialog;

    NamedPipeClientTransport* _pClientTransport;
    Channel* _pClientChannel;
    GameCallbacks* _pGameCallbacks;
    PresentationCallbacks* _pPresentationCallbacks;

    ServiceCallbacks* _pServiceCallbacks;
    RuntimeServiceCallback* _pRuntimeServiceCallbacks;
    RuntimePresentationServiceCallback* _pRuntimePresentationServiceCallbacks;
    NamedPipeServerTransport* _pServerTransport;
    Server* _pServer;
};
