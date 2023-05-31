#pragma once

#include "LogCallback.h"
#include "RuntimePresentationCallbacks.h"
#include "runtimePresentationService.snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class RuntimePresentationServiceCallback : public IRuntimePresentationServiceCallback
{
    LogCallback* _pLog;
    RuntimePresentationCallbacks* _pGamesideRuntimePresentationCallbacks;
public:
    RuntimePresentationServiceCallback(LogCallback* pLogCallback, RuntimePresentationCallbacks* pGamesideRuntimePresentationCallbacks)
    : _pLog(pLogCallback)
    , _pGamesideRuntimePresentationCallbacks(pGamesideRuntimePresentationCallbacks)
    {}
    virtual StatusCode PresentOverriddenPresentation(OverriddenPresentationMessage& request, OverriddenPresentationResponse& response, Status& status) { _pLog->Log(LogInfo, "RuntimePresentationServiceCallback", "Called PresentOverriddenPresentation"); return OK; }
};
