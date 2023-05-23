#pragma once

#include "GameCallbacks.h" 
#include "LogCallback.h"
#include "presentationService.snapp.pb.h"

using namespace Aristocrat::Snapp;
using namespace Aristocrat::GdkRuntime::v1;

class PresentationCallbacks
{
    PresentationServiceStub* _stub;
    GameCallbacks* _pGameCallbacks;
    LogCallback* _pLog;
public:
    PresentationCallbacks(GameCallbacks* pGameCallbacks, LogCallback* pLog, Channel* channel)
        : _stub(new PresentationServiceStub(channel))
        , _pGameCallbacks(pGameCallbacks)
        , _pLog(pLog)
    {
    }

    ~PresentationCallbacks()
    {
        delete _stub;
    }

    void Log(::LogLevel level, const char* logText)
    {
        if (_pLog)
        {
            _pLog->Log(level, "PresentationCallbacks", logText);
        }
    }

    void RegisterPresentationOverrides(RegisterPresentationRequest& request, RegisterPresentationResponse& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "RegisterPresentationOverride()");
            _stub->RegisterPresentation(request, response, status);
        }
    }
};
