#pragma once

#include "GameCallbacks.h" 
#include "LogCallback.h"
#include "runtimeService.snapp.pb.h"

using namespace Aristocrat::Snapp;
using namespace Aristocrat::GdkRuntime::v1;

class RuntimeCallbacks
{
    RuntimeServiceStub* _stub;
    GameCallbacks* _pGameCallbacks;
    LogCallback* _pLog;
public:
    RuntimeCallbacks(GameCallbacks* pGameCallbacks, LogCallback* pLog, Channel* channel)
        : _stub(new RuntimeServiceStub(channel))
        , _pGameCallbacks(pGameCallbacks)
        , _pLog(pLog)
    {
    }

    ~RuntimeCallbacks()
    {
        delete _stub;
    }

    void Log(::LogLevel level, const char* logText)
    {
        if (_pLog)
        {
            _pLog->Log(level, "RuntimeCallbacks", logText);
        }
    }

    void GetState(Empty& request, GetStateResponse& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "GetState()");
            _stub->GetState(request, response, status);
        }
    }

    void UpdateState(UpdateStateRequest& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateState()");
            _stub->UpdateState(request, response, status);
        }
    }

    void GetFlag(GetFlagRequest& request, GetFlagResponse& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "GetFlag()");
            _stub->GetFlag(request, response, status);
        }
    }

    void UpdateFlag(UpdateFlagRequest& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateFlag()");
            _stub->UpdateFlag(request, response, status);
        }
    }

    void UpdateParameters(UpdateParametersRequest& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateParameters()");
            _stub->UpdateParameters(request, response, status);
        }
    }

    void UpdateLocalTimeTranslationBias(UpdateLocalTimeTranslationBiasRequest& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateLocalTimeTranslationBias()");
            _stub->UpdateLocalTimeTranslationBias(request, response, status);
        }
    }

    void UpdateButtonState(UpdateButtonStateRequest& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateButtonState()");
            _stub->UpdateButtonState(request, response, status);
        }
    }

    void UpdateBalance(UpdateBalanceNotification& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateBalance()");
            _stub->UpdateBalance(request, response, status);
        }
    }

    void UpdatePlatformMessage(UpdatePlatformMessageRequest& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdatePlatformMessage()");
            _stub->UpdatePlatformMessage(request, response, status);
        }
    }

    void UpdateVolume(VolumeUpdateNotification& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateVolume()");
            _stub->UpdateVolume(request, response, status);
        }
    }

    void UpdateTimeRemaining(UpdateTimeRemainingRequest& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateTimeRemaining()");
            _stub->UpdateTimeRemaining(request, response, status);
        }
    }

    void UpdateHandCount(UpdateHandCountNotification& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateHandCount()");
            _stub->UpdateHandCount(request, response, status);
        }
    }

    void InvokeButton(InvokeButtonRequest& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "InvokeButton()");
            _stub->InvokeButton(request, response, status);
        }
    }

    void BeginGameRoundResult(BeginGameRoundNotification& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "BeginGameRoundResult()");
            _stub->BeginGameRoundResult(request, response, status);
        }
    }

    void OnJackpotUpdated(Empty& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "OnJackpotUpdated()");
            _stub->OnJackpotUpdated(request, response, status);
        }
    }

    void JackpotWinAvailable(JackpotWinAvailableNotification& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "JackpotWinAvailable()");
            _stub->JackpotWinAvailable(request, response, status);
        }
    }

    void Shutdown(Empty& request, Empty& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "Shutdown()");
            _stub->Shutdown(request, response, status);
        }
    }
};
