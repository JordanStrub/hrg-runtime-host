#pragma once

#include "LogCallback.h"
#include "gameService.snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class GameServiceCallback : public IGameServiceCallback
{
    LogCallback* _pLog;
public:
    GameServiceCallback(LogCallback* pLogCallback) : _pLog(pLogCallback) {}
    virtual StatusCode Join(JoinRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called Join"); return OK; }
    virtual StatusCode Leave(LeaveRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called Leave"); return OK; }
    virtual StatusCode FatalError(FatalErrorNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called FatalError"); return OK; }
    virtual StatusCode RuntimeEvent(RuntimeEventNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called RuntimeEvent"); return OK; }
    virtual StatusCode RuntimeFlagChange(RuntimeFlagNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called RuntimeFlagChange"); return OK; }
    virtual StatusCode RuntimeRequest(RuntimeRequestNotification& request, RuntimeRequestResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called RuntimeRequest"); return OK; }
    virtual StatusCode RuntimeStateChange(RuntimeStateNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called RuntimeStateChange"); return OK; }
    virtual StatusCode ButtonDeckImageChanged(Empty& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called ButtonDeckImageChanged"); return OK; }
    virtual StatusCode ButtonStatesChanged(ButtonStatesChangedNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called ButtonStatesChanged"); return OK; }
    virtual StatusCode BeginGameRound(BeginGameRoundRequest& request, BeginGameRoundResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called BeginGameRound"); return OK; }
    virtual StatusCode BeginGameRoundAsync(BeginGameRoundAsyncRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called BeginGameRoundAsync"); return OK; }
    virtual StatusCode BeginGameRoundResult(BeginGameRoundResultNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called BeginGameRoundResult"); return OK; }
    virtual StatusCode GameRoundEvent(GameRoundEventRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called GameRoundEvent"); return OK; }
    virtual StatusCode RecoveryPoint(RecoveryPointNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called RecoveryPoint"); return OK; }
    virtual StatusCode GetLocalStorage(GetLocalStorageRequest& request, GetLocalStorageResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called GetLocalStorage"); return OK; }
    virtual StatusCode UpdateLocalStorage(UpdateLocalStorageRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called UpdateLocalStorage"); return OK; }
    virtual StatusCode GetMeters(Empty& request, GetMetersResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called GetMeters"); return OK; }
    virtual StatusCode UpdateMeters(UpdateMetersRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called UpdateMeters"); return OK; }
    virtual StatusCode GetRandomNumber32(GetRandomNumber32Request& request, GetRandomNumber32Response& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called GetRandomNumber32"); return OK; }
    virtual StatusCode GetRandomNumber64(GetRandomNumber64Request& request, GetRandomNumber64Response& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called GetRandomNumber64"); return OK; }
    virtual StatusCode Shuffle(ShuffleRequest& request, ShuffleResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called Shuffle"); return OK; }
    virtual StatusCode UpdateBonusKey(UpdateBonusKeyRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called UpdateBonusKey"); return OK; }
    virtual StatusCode EndGameRound(EndGameRoundRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called EndGameRound"); return OK; }
    virtual StatusCode UpdateBetOptions(UpdateBetOptionsRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called UpdateBetOptions"); return OK; }
    virtual StatusCode ConnectJackpotPool(ConnectJackpotPoolRequest& request, ConnectJackpotPoolResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called ConnectJackpotPool"); return OK; }
    virtual StatusCode GetJackpotValues(GetJackpotValuesRequest& request, LevelInfoResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called GetJackpotValues"); return OK; }
    virtual StatusCode UpdateJackpotValues(UpdateJackpotValuesRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called UpdateJackpotValues"); return OK; }
    virtual StatusCode TriggerJackpot(TriggerJackpotRequest& request, TriggerJackpotResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called TriggerJackpot"); return OK; }
    virtual StatusCode ClaimJackpot(ClaimJackpotRequest& request, LevelInfoResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called ClaimJackpot"); return OK; }
    virtual StatusCode SetJackpotLevelWagers(LevelWagerRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called SetJackpotLevelWagers"); return OK; }
    virtual StatusCode CheckMysteryJackpot(CheckMysteryJackpotRequest& request, CheckMysteryJackpotResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called CheckMysteryJackpot"); return OK; }
    virtual StatusCode SelectDenomination(SelectDenominationRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback", "Called SelectDenomination"); return OK; }
};
