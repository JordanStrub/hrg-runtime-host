#pragma once

#include "LogCallback.h"
#include "Runtime.h"
#include "RuntimeCallbacks.h"
#include "gameService.snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class GameServiceCallback : public IGameServiceCallback
{
    LogCallback* _pLog;
    Runtime* _pRuntime;
    RuntimeCallbacks* _pRuntimeCallbacks;
    Channel* _pClientChannel;
    GameCallbacks* _pPlatformGameCallbacks;
public:
    GameServiceCallback(LogCallback* pLogCallback, Runtime* pRuntime, RuntimeCallbacks* pRuntimeCallbacks, Channel* pClientChannel,
        GameCallbacks* pPlatformGameCallbacks)
    : _pLog(pLogCallback)
    , _pRuntime(pRuntime)
    , _pRuntimeCallbacks(pRuntimeCallbacks)
    , _pClientChannel(pClientChannel)
    , _pPlatformGameCallbacks(pPlatformGameCallbacks)
    {
    }
    virtual StatusCode Join(JoinRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called Join");

        _pClientChannel->Connect();
        _pLog->Log(LogInfo, "CommPlugin", "Started game-side client comms");

        _pPlatformGameCallbacks->Join(request, response, status);
        return status.status_code();
    }
    virtual StatusCode Leave(LeaveRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called Leave");
        _pPlatformGameCallbacks->Leave(request, response, status);
        return status.status_code();
    }
    virtual StatusCode FatalError(FatalErrorNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called FatalError");
        _pPlatformGameCallbacks->FatalError(request, response, status);
        return status.status_code();
    }
    virtual StatusCode RuntimeEvent(RuntimeEventNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RuntimeEvent");
        _pPlatformGameCallbacks->RuntimeEvent(request, response, status);
        return status.status_code();
    }
    virtual StatusCode RuntimeFlagChange(RuntimeFlagNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RuntimeFlagChange");
        _pPlatformGameCallbacks->RuntimeFlagChange(request, response, status);
        return status.status_code();
    }
    virtual StatusCode RuntimeRequest(RuntimeRequestNotification& request, RuntimeRequestResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RuntimeRequest");
        _pPlatformGameCallbacks->RuntimeRequest(request, response, status);
        return status.status_code();
    }
    virtual StatusCode RuntimeStateChange(RuntimeStateNotication& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RuntimeStateChange");
        _pPlatformGameCallbacks->RuntimeStateChange(request, response, status);
        return status.status_code();
    }
    virtual StatusCode ButtonDeckImageChanged(Empty& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called ButtonDeckImageChanged");
        _pPlatformGameCallbacks->ButtonDeckImageChanged(request, response, status);
        return status.status_code();
    }
    virtual StatusCode ButtonStatesChanged(ButtonStatesChangedNotfication& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called ButtonStatesChanged");
        _pPlatformGameCallbacks->ButtonStatesChanged(request, response, status);
        return status.status_code();
    }
    virtual StatusCode BeginGameRound(BeginGameRoundRequest& request, BeginGameRoundResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called BeginGameRound");
        _pPlatformGameCallbacks->BeginGameRound(request, response, status);
        return status.status_code();
    }
    virtual StatusCode BeginGameRoundAsync(BeginGameRoundAsyncRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called BeginGameRoundAsync");
        _pPlatformGameCallbacks->BeginGameRoundAsync(request, response, status);
        return status.status_code();
    }
    virtual StatusCode BeginGameRoundResult(BeginGameRoundResultNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called BeginGameRoundResult");
        _pPlatformGameCallbacks->BeginGameRoundResult(request, response, status);
        return status.status_code();
    }
    virtual StatusCode GameRoundEvent(GameRoundEventRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GameRoundEvent");
        _pPlatformGameCallbacks->GameRoundEvent(request, response, status);
        return status.status_code();
    }
    virtual StatusCode RecoveryPoint(RecoveryPointNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RecoveryPoint");
        _pPlatformGameCallbacks->RecoveryPoint(request, response, status);
        return status.status_code();
    }
    virtual StatusCode GetLocalStorage(GetLocalStorageRequest& request, GetLocalStorageResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetLocalStorage");
        _pPlatformGameCallbacks->GetLocalStorage(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateLocalStorage(UpdateLocalStorageRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateLocalStorage");
        _pPlatformGameCallbacks->UpdateLocalStorage(request, response, status);
        return status.status_code();
    }
    virtual StatusCode GetMeters(Empty& request, GetMetersResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetMeters");
        _pPlatformGameCallbacks->GetMeters(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateMeters(UpdateMetersRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateMeters");
        _pPlatformGameCallbacks->UpdateMeters(request, response, status);
        return status.status_code();
    }
    virtual StatusCode GetRandomNumber32(GetRandomNumber32Request& request, GetRandomNumber32Response& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetRandomNumber32");
        _pPlatformGameCallbacks->GetRandomNumber32(request, response, status);
        return status.status_code();
    }
    virtual StatusCode GetRandomNumber64(GetRandomNumber64Request& request, GetRandomNumber64Response& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetRandomNumber64");
        _pPlatformGameCallbacks->GetRandomNumber64(request, response, status);
        return status.status_code();
    }
    virtual StatusCode Shuffle(ShuffleRequest& request, ShuffleResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called Shuffle");
        _pPlatformGameCallbacks->Shuffle(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateBonusKey(UpdateBonusKeyRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateBonusKey");
        _pPlatformGameCallbacks->UpdateBonusKey(request, response, status);
        return status.status_code();
    }
    virtual StatusCode EndGameRound(EndGameRoundRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called EndGameRound");
        _pPlatformGameCallbacks->EndGameRound(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateBetOptions(UpdateBetOptionsRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateBetOptions");
        _pPlatformGameCallbacks->UpdateBetOptions(request, response, status);
        return status.status_code();
    }
    virtual StatusCode ConnectJackpotPool(ConnectJackpotPoolRequest& request, ConnectJackpotPoolResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called ConnectJackpotPool");
        _pPlatformGameCallbacks->ConnectJackpotPool(request, response, status);
        return status.status_code();
    }
    virtual StatusCode GetJackpotValues(GetJackpotValuesRequest& request, LevelInfoResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetJackpotValues");
        _pPlatformGameCallbacks->GetJackpotValues(request, response, status);
        return status.status_code();
    }
    virtual StatusCode UpdateJackpotValues(UpdateJackpotValuesRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateJackpotValues");
        _pPlatformGameCallbacks->UpdateJackpotValues(request, response, status);
        return status.status_code();
    }
    virtual StatusCode TriggerJackpot(TriggerJackpotRequest& request, TriggerJackpotResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called TriggerJackpot");
        _pPlatformGameCallbacks->TriggerJackpot(request, response, status);
        return status.status_code();
    }
    virtual StatusCode ClaimJackpot(ClaimJackpotRequest& request, LevelInfoResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called ClaimJackpot");
        _pPlatformGameCallbacks->ClaimJackpot(request, response, status);
        return status.status_code();
    }
    virtual StatusCode SetJackpotLevelWagers(LevelWagerRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called SetJackpotLevelWagers");
        _pPlatformGameCallbacks->SetJackpotLevelWagers(request, response, status);
        return status.status_code();
    }
    virtual StatusCode CheckMysteryJackpot(CheckMysteryJackpotRequest& request, CheckMysteryJackpotResponse& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called CheckMysteryJackpot");
        _pPlatformGameCallbacks->CheckMysteryJackpot(request, response, status);
        return status.status_code();
    }
    virtual StatusCode SelectDenomination(SelectDenominationRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called SelectDenomination");
        _pPlatformGameCallbacks->SelectDenomination(request, response, status);
        return status.status_code();
    }
};
