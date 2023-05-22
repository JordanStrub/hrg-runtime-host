#pragma once

#include "LogCallback.h"
#include "runtimeService.snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class RuntimeServiceCallback : public IRuntimeServiceCallback
{
    LogCallback* _pLog;
public:
    RuntimeServiceCallback(LogCallback* pLogCallback) : _pLog(pLogCallback) {}
    virtual StatusCode GetState(Empty& request, GetStateResponse& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called GetState"); return OK; };
    virtual StatusCode UpdateState(UpdateStateRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdateState"); return OK; };
    virtual StatusCode GetFlag(GetFlagRequest& request, GetFlagResponse& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called GetFlag"); return OK; };
    virtual StatusCode UpdateFlag(UpdateFlagRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdateFlag"); return OK; };
    virtual StatusCode UpdateParameters(UpdateParametersRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdateParameters"); return OK; };
    virtual StatusCode UpdateLocalTimeTranslationBias(UpdateLocalTimeTranslationBiasRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdateLocalTimeTranslationBias"); return OK; };
    virtual StatusCode UpdateButtonState(UpdateButtonStateRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdateButtonState"); return OK; };
    virtual StatusCode UpdateBalance(UpdateBalanceNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdateBalance"); return OK; };
    virtual StatusCode UpdatePlatformMessage(UpdatePlatformMessageRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdatePlatformMessage"); return OK; };
    virtual StatusCode UpdateVolume(VolumeUpdateNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdateVolume"); return OK; };
    virtual StatusCode UpdateTimeRemaining(UpdateTimeRemainingRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called UpdateTimeRemaining"); return OK; };
    virtual StatusCode UpdateHandCount(UpdateHandCountNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback", "UpdateHandCount"); return OK; }
    virtual StatusCode InvokeButton(InvokeButtonRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called InvokeButton"); return OK; };
    virtual StatusCode BeginGameRoundResult(BeginGameRoundNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called BeginGameRoundResult"); return OK; };
    virtual StatusCode OnJackpotUpdated(Empty& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called OnJackpotUpdated"); return OK; };
    virtual StatusCode JackpotWinAvailable(JackpotWinAvailableNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called JackpotWinAvailable"); return OK; };
    virtual StatusCode Shutdown(Empty& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeServiceCallback","Called Shutdown"); return OK; };
};

