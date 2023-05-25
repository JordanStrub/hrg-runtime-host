#pragma once

#include "GameCallbacks.h"
#include "LogCallback.h"
#include "reelService.snapp.pb.h"

using namespace Aristocrat::Snapp;
using namespace Aristocrat::GdkRuntime::v1;

class ReelCallbacks
{
    ReelServiceStub* _stub;
    GameCallbacks* _pGameCallbacks;
    LogCallback* _pLog;
public:
    ReelCallbacks(GameCallbacks* pGameCallbacks, LogCallback* pLog, Channel* channel)
        : _stub(new ReelServiceStub(channel))
        , _pGameCallbacks(pGameCallbacks)
        , _pLog(pLog)
    {
    }

    ~ReelCallbacks()
    {
        delete _stub;
    }

    void Log(::LogLevel level, const char* logText)
    {
        if (_pLog)
        {
            _pLog->Log(level, "ReelCallbacks", logText);
        }
    }

    void SpinReels(SpinReelsRequest& request, SpinReelsResponse& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "SpinReels()");
            _stub->SpinReels(request, response, status);
        }
    }

    void NudgeReels(NudgeReelsRequest& request, NudgeReelsResponse& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "NudgeReels()");
            _stub->NudgeReels(request, response, status);
        }
    }

    void GetReelsState(GetReelsStateRequest& request, GetReelsStateResponse& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "GetReelsState()");
            _stub->GetReelsState(request, response, status);
        }
    }

    void GetConnectedReels(ConnectedReelsRequest& request, ConnectedReelsResponse& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "GetConnectedReels()");
            _stub->GetConnectedReels(request, response, status);
        }
    }

    void UpdateReelsSpeed(UpdateReelsSpeedRequest& request, UpdateReelsSpeedResponse& response, Status& status)
    {
        if (_pGameCallbacks->is_communication_open())
        {
            Log(LogInfo, "UpdateReelsSpeed()");
            _stub->UpdateReelsSpeed(request, response, status);
        }
    }
};
