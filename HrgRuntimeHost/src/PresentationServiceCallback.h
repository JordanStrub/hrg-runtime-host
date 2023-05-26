#pragma once

#include "LogCallback.h"
#include "presentationService.snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class PresentationServiceCallback : public IPresentationServiceCallback
{
    LogCallback* _pLog;
public:
    PresentationServiceCallback(LogCallback* pLogCallback) : _pLog(pLogCallback) {}
    virtual StatusCode RegisterPresentation(RegisterPresentationRequest& request, RegisterPresentationResponse& response, Status& status) { _pLog->Log(LogInfo, "PresentationServiceCallback from game", "Called RegisterPresentation"); return OK; }
};
