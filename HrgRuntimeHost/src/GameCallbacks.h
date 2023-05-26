#pragma once

#include "LogCallback.h"
#include "gameService.snapp.pb.h"

using namespace Aristocrat::Snapp;
using namespace Aristocrat::GdkRuntime::v1;

class GameCallbacks
{
    bool _communication_open;
    Channel* _channel;
    GameServiceStub* _stub;
    LogCallback* _pLog;
public:
    GameCallbacks(LogCallback* pLog, Channel* channel)
        : _channel(channel)
        , _stub(new GameServiceStub(channel))
        , _pLog(pLog)
        , _communication_open(true)
    {
    }

    ~GameCallbacks()
    {
        delete _stub;
    }

    void Log(::LogLevel level, const char* logText)
    {
        if (_pLog)
        {
            _pLog->Log(level, "GameCallbacks", logText);
        }
    }

    bool is_communication_open()
    {
        if (!_communication_open)
        {
            Log(LogDebug, "OUTGOING COMMUNICATION IS CLOSED.");
        }

        return _communication_open;
    }

    void set_communication_open(bool open)
    {
        _communication_open = open;
    }

    void Join(JoinRequest& request, Empty& response, Status& status)
    {
        Log(LogInfo, "Join");
        _stub->Join(request, response, status);

        char buf[1000];
        sprintf(buf, "Join: status = %d %s", status.status_code(), status.error_description().c_str());
        Log(LogInfo, buf);
    }

    void Leave(LeaveRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "Leave");
            _stub->Leave(request, response, status);
            _communication_open = false;

            Log(LogInfo, "Snapp Connection closed.");
        }
        else
        {
            Log(LogInfo, "Snapp Connection already lost.");
        }
    }

    void FatalError(FatalErrorNotification& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "FatalError()");
            _stub->FatalError(request, response, status);
        }
    }

    void RuntimeEvent(RuntimeEventNotification& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "RuntimeEvent");
            _stub->RuntimeEvent(request, response, status);
        }
    }

    void RuntimeFlagChange(RuntimeFlagNotification& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "RuntimeFlagChange");
            _stub->RuntimeFlagChange(request, response, status);
        }
    }

    void RuntimeRequest(RuntimeRequestNotification& request, RuntimeRequestResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "RuntimeRequest()");
            _stub->RuntimeRequest(request, response, status);
        }
    }

    void RuntimeStateChange(RuntimeStateNotication& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "RuntimeStateChange()");
            _stub->RuntimeStateChange(request, response, status);
        }
    }

    void ButtonDeckImageChanged(Empty& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "ButtonDeckImageChanged()");
            _stub->ButtonDeckImageChanged(request, response, status);
        }
    }

    void ButtonStatesChanged(ButtonStatesChangedNotfication& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "ButtonStatesChanged()");
            _stub->ButtonStatesChanged(request, response, status);
        }
    }

    void BeginGameRound(BeginGameRoundRequest& request, BeginGameRoundResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "BeginGameRound() [SYNC]");
            _stub->BeginGameRound(request, response, status);
        }
    }

    void BeginGameRoundAsync(BeginGameRoundAsyncRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "BeginGameRoundAsync()");
            _stub->BeginGameRoundAsync(request, response, status);
        }
    }

    void BeginGameRoundResult(BeginGameRoundResultNotification& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "BeginGameRoundResult()");
            _stub->BeginGameRoundResult(request, response, status);
        }
    }

    void GameRoundEvent(GameRoundEventRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "GameRoundEvent");
            _stub->GameRoundEvent(request, response, status);
        }
    }

    void RecoveryPoint(RecoveryPointNotification& request, Empty& response, Status& status)
    {

        if (is_communication_open())
        {
            Log(LogInfo, "RecoveryPoint()");
            _stub->RecoveryPoint(request, response, status);
        }
    }

    void GetLocalStorage(GetLocalStorageRequest& request, GetLocalStorageResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "GetLocalStorage");
            _stub->GetLocalStorage(request, response, status);
        }
    }

    void UpdateLocalStorage(UpdateLocalStorageRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "UpdateLocalStorage");
            _stub->UpdateLocalStorage(request, response, status);
        }
    }

    void GetMeters(Empty& request, GetMetersResponse& response, Status& status)
    {
        {
            Log(LogInfo, "GetMeters");
            _stub->GetMeters(request, response, status);
        }
    }

    void UpdateMeters(UpdateMetersRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "UpdateMeters");
            _stub->UpdateMeters(request, response, status);
        }
    }

    void GetRandomNumber32(GetRandomNumber32Request& request, GetRandomNumber32Response& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "GetRandomNumber32()");
            _stub->GetRandomNumber32(request, response, status);
        }
    }

    void GetRandomNumber64(GetRandomNumber64Request& request, GetRandomNumber64Response& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "GetRandomNumber64()");
            _stub->GetRandomNumber64(request, response, status);
        }
    }

    void Shuffle(ShuffleRequest& request, ShuffleResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "Shuffle");
            _stub->Shuffle(request, response, status);
        }
    }

    void UpdateBonusKey(UpdateBonusKeyRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "UpdateBonusKey");
            _stub->UpdateBonusKey(request, response, status);
        }
    }

    void EndGameRound(EndGameRoundRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "EndGameRound()");
            _stub->EndGameRound(request, response, status);
        }
    }

    void UpdateBetOptions(UpdateBetOptionsRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "UpdateBetOptions()");
            _stub->UpdateBetOptions(request, response, status);
        }
    }

    void ConnectJackpotPool(ConnectJackpotPoolRequest& request, ConnectJackpotPoolResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "ConnectJackpotPool()");
            _stub->ConnectJackpotPool(request, response, status);
        }
    }

    void GetJackpotValues(GetJackpotValuesRequest& request, LevelInfoResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "GetJackpotValues()");
            _stub->GetJackpotValues(request, response, status);
        }
    }

    void UpdateJackpotValues(UpdateJackpotValuesRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "UpdateJackpotValues()");
            _stub->UpdateJackpotValues(request, response, status);
        }
    }

    void TriggerJackpot(TriggerJackpotRequest& request, TriggerJackpotResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "TriggerJackpot()");
            _stub->TriggerJackpot(request, response, status);
        }
    }

    void ClaimJackpot(ClaimJackpotRequest& request, LevelInfoResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "ClaimJackpot()");
            _stub->ClaimJackpot(request, response, status);
        }
    }

    void SetJackpotLevelWagers(LevelWagerRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "SetJackpotLevelWagers()");
            _stub->SetJackpotLevelWagers(request, response, status);
        }
    }

    void CheckMysteryJackpot(CheckMysteryJackpotRequest& request, CheckMysteryJackpotResponse& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "CheckMysteryJackpot()");
            _stub->CheckMysteryJackpot(request, response, status);
        }
    }

    void SelectDenomination(SelectDenominationRequest& request, Empty& response, Status& status)
    {
        if (is_communication_open())
        {
            Log(LogInfo, "SelectDenomination()");
            _stub->SelectDenomination(request, response, status);
        }
    }
};
