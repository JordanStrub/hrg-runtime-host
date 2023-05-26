#pragma once

#include "RuntimeCallbacks.h" 
#include "LogCallback.h"
#include "runtimePresentationService.snapp.pb.h"

using namespace Aristocrat::Snapp;
using namespace Aristocrat::GdkRuntime::v1;

class RuntimePresentationCallbacks
{
    RuntimePresentationServiceStub* _stub;
    RuntimeCallbacks* _pRuntimeCallbacks;
    LogCallback* _pLog;
public:
    RuntimePresentationCallbacks(RuntimeCallbacks* pRuntimeCallbacks, LogCallback* pLog, Channel* channel)
        : _stub(new RuntimePresentationServiceStub(channel))
        , _pRuntimeCallbacks(pRuntimeCallbacks)
        , _pLog(pLog)
    {
    }

    ~RuntimePresentationCallbacks()
    {
        delete _stub;
    }

    void Log(::LogLevel level, const char* logText)
    {
        if (_pLog)
        {
            _pLog->Log(level, "RuntimePresentationCallbacks", logText);
        }
    }

    void PresentOverriddenPresentation(OverriddenPresentationMessage& request, OverriddenPresentationResponse& response, Status& status)
    {
        if (_pRuntimeCallbacks->is_communication_open())
        {
            Log(LogInfo, "RegisterPresentationOverride()");
            _stub->PresentOverriddenPresentation(request, response, status);
        }
    }
};
