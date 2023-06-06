#pragma once

#include "LogCallback.h"
#include "PresentationCallbacks.h"
#include "presentationService.snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class PresentationServiceCallback : public IPresentationServiceCallback
{
    LogCallback* _pLog;
    PresentationCallbacks* _pPlatformPresentationCallbacks;
public:
    PresentationServiceCallback(LogCallback* pLogCallback, PresentationCallbacks* pPresentationCallbacks)
    : _pLog(pLogCallback)
    , _pPlatformPresentationCallbacks(pPresentationCallbacks)
    {}
    virtual StatusCode RegisterPresentation(RegisterPresentationRequest& request, RegisterPresentationResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "PresentationServiceCallback from game", "Called RegisterPresentation");
        _pPlatformPresentationCallbacks->RegisterPresentationOverrides(request, response, status);
        return status.status_code();
    }
};
