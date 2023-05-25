#pragma once

#include "LogCallback.h"
#include "runtimePresentationService.snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class RuntimePresentationServiceCallback : public IRuntimePresentationServiceCallback
{
    LogCallback* _pLog;
public:
    RuntimePresentationServiceCallback(LogCallback* pLogCallback) : _pLog(pLogCallback) {}
    virtual StatusCode PresentOverriddenPresentation(OverriddenPresentationMessage& request, OverriddenPresentationResponse& response, Status& status) { _pLog->Log(LogInfo, "RuntimePresentationServiceCallback", "Called PresentOverriddenPresentation"); return OK; }
};
