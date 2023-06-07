#pragma once

#include "LogCallback.h"
#include "RuntimeCallbacks.h"
#include "runtimeService.snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class RuntimeServiceCallback : public IRuntimeServiceCallback
{
    LogCallback* _pLog;
    RuntimeCallbacks* _pGamesideRuntimeCallbacks;
public:
    RuntimeServiceCallback(LogCallback* pLogCallback, RuntimeCallbacks* pGamesideRuntimeCallbacks)
    : _pLog(pLogCallback)
    , _pGamesideRuntimeCallbacks(pGamesideRuntimeCallbacks)
    {}
    virtual StatusCode GetState(Empty& request, GetStateResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called GetState");
        _pGamesideRuntimeCallbacks->GetState(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateState(UpdateStateRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdateState");
        _pGamesideRuntimeCallbacks->UpdateState(request, response, status);
        return status.status_code();
    }
    virtual StatusCode GetFlag(GetFlagRequest& request, GetFlagResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called GetFlag");
        _pGamesideRuntimeCallbacks->GetFlag(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateFlag(UpdateFlagRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdateFlag");
        _pGamesideRuntimeCallbacks->UpdateFlag(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateParameters(UpdateParametersRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdateParameters");
        char buf[1000];
        auto map = request.parameters();
        for (auto& pair : map)
        {
            sprintf(buf, "parameter %s = %s", pair.first.c_str(), pair.second.c_str());
            _pLog->Log(LogInfo, "RuntimeServiceCallback from platform", buf);
        }

        _pGamesideRuntimeCallbacks->UpdateParameters(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateLocalTimeTranslationBias(UpdateLocalTimeTranslationBiasRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdateLocalTimeTranslationBias");
        _pGamesideRuntimeCallbacks->UpdateLocalTimeTranslationBias(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateButtonState(UpdateButtonStateRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdateButtonState");
        _pGamesideRuntimeCallbacks->UpdateButtonState(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateBalance(UpdateBalanceNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdateBalance");
        _pGamesideRuntimeCallbacks->UpdateBalance(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdatePlatformMessage(UpdatePlatformMessageRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdatePlatformMessage");
        _pGamesideRuntimeCallbacks->UpdatePlatformMessage(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateVolume(VolumeUpdateNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdateVolume");
        _pGamesideRuntimeCallbacks->UpdateVolume(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateTimeRemaining(UpdateTimeRemainingRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called UpdateTimeRemaining");
        _pGamesideRuntimeCallbacks->UpdateTimeRemaining(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateHandCount(UpdateHandCountNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform", "UpdateHandCount");
        _pGamesideRuntimeCallbacks->UpdateHandCount(request, response, status);
        return status.status_code();
    }
    virtual StatusCode InvokeButton(InvokeButtonRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called InvokeButton");
        _pGamesideRuntimeCallbacks->InvokeButton(request, response, status);
        return status.status_code();
    }
    virtual StatusCode BeginGameRoundResult(BeginGameRoundNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called BeginGameRoundResult");
        _pGamesideRuntimeCallbacks->BeginGameRoundResult(request, response, status);
        return status.status_code();
    }
    virtual StatusCode OnJackpotUpdated(Empty& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called OnJackpotUpdated");
        _pGamesideRuntimeCallbacks->OnJackpotUpdated(request, response, status);
        return status.status_code();
    }
    virtual StatusCode JackpotWinAvailable(JackpotWinAvailableNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called JackpotWinAvailable");
        _pGamesideRuntimeCallbacks->JackpotWinAvailable(request, response, status);
        return status.status_code();
    }
    virtual StatusCode Shutdown(Empty& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "RuntimeServiceCallback from platform","Called Shutdown");
        _pGamesideRuntimeCallbacks->Shutdown(request, response, status);
        return status.status_code();
    }
};

