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
public:
    GameServiceCallback(LogCallback* pLogCallback, Runtime* pRuntime, RuntimeCallbacks* pRuntimeCallbacks, Channel* pClientChannel)
    : _pLog(pLogCallback)
    , _pRuntime(pRuntime)
    , _pRuntimeCallbacks(pRuntimeCallbacks)
    , _pClientChannel(pClientChannel)
    {
    }
    virtual StatusCode Join(JoinRequest& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called Join");

        _pClientChannel->Connect();
        _pLog->Log(LogInfo, "CommPlugin", "Started game-side client comms");

        return OK;
    }
    virtual StatusCode Leave(LeaveRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called Leave"); return OK; }
    virtual StatusCode FatalError(FatalErrorNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called FatalError"); return OK; }
    virtual StatusCode RuntimeEvent(RuntimeEventNotification& request, Empty& response, Status& status)
    {
        _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RuntimeEvent");

        switch (request.runtimeevent())
        {
        case RuntimeEventNotification::RequestConfiguration:
            {
                _pLog->Log(LogDebug, "...", "RuntimeEventNotification::RequestConfiguration");
                // Send the Config parameters derived from command line.
                UpdateParametersRequest parametersRequest;
                parametersRequest.set_target(GameConfiguration);
                auto params = _pRuntime->GetConfigParameters();
                for (const auto& pair : params)
                {
                    (*parametersRequest.mutable_parameters())[pair.first] = pair.second;
                }
                _pRuntimeCallbacks->UpdateParameters(parametersRequest, response, status);
                _pLog->Log(LogDebug, "...", "called callbacks->UpdateParameters");
        }
            break;
        default:
            _pLog->Log(LogDebug, "...", "NOT RuntimeEventNotification::RequestConfiguration");
            break;
        }

        return OK;
    }
    virtual StatusCode RuntimeFlagChange(RuntimeFlagNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RuntimeFlagChange"); return OK; }
    virtual StatusCode RuntimeRequest(RuntimeRequestNotification& request, RuntimeRequestResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RuntimeRequest"); return OK; }
    virtual StatusCode RuntimeStateChange(RuntimeStateNotication& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RuntimeStateChange"); return OK; }
    virtual StatusCode ButtonDeckImageChanged(Empty& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called ButtonDeckImageChanged"); return OK; }
    virtual StatusCode ButtonStatesChanged(ButtonStatesChangedNotfication& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called ButtonStatesChanged"); return OK; }
    virtual StatusCode BeginGameRound(BeginGameRoundRequest& request, BeginGameRoundResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called BeginGameRound"); return OK; }
    virtual StatusCode BeginGameRoundAsync(BeginGameRoundAsyncRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called BeginGameRoundAsync"); return OK; }
    virtual StatusCode BeginGameRoundResult(BeginGameRoundResultNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called BeginGameRoundResult"); return OK; }
    virtual StatusCode GameRoundEvent(GameRoundEventRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GameRoundEvent"); return OK; }
    virtual StatusCode RecoveryPoint(RecoveryPointNotification& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called RecoveryPoint"); return OK; }
    virtual StatusCode GetLocalStorage(GetLocalStorageRequest& request, GetLocalStorageResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetLocalStorage"); return OK; }
    virtual StatusCode UpdateLocalStorage(UpdateLocalStorageRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateLocalStorage"); return OK; }
    virtual StatusCode GetMeters(Empty& request, GetMetersResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetMeters"); return OK; }
    virtual StatusCode UpdateMeters(UpdateMetersRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateMeters"); return OK; }
    virtual StatusCode GetRandomNumber32(GetRandomNumber32Request& request, GetRandomNumber32Response& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetRandomNumber32"); return OK; }
    virtual StatusCode GetRandomNumber64(GetRandomNumber64Request& request, GetRandomNumber64Response& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetRandomNumber64"); return OK; }
    virtual StatusCode Shuffle(ShuffleRequest& request, ShuffleResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called Shuffle"); return OK; }
    virtual StatusCode UpdateBonusKey(UpdateBonusKeyRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateBonusKey"); return OK; }
    virtual StatusCode EndGameRound(EndGameRoundRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called EndGameRound"); return OK; }
    virtual StatusCode UpdateBetOptions(UpdateBetOptionsRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateBetOptions"); return OK; }
    virtual StatusCode ConnectJackpotPool(ConnectJackpotPoolRequest& request, ConnectJackpotPoolResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called ConnectJackpotPool"); return OK; }
    virtual StatusCode GetJackpotValues(GetJackpotValuesRequest& request, LevelInfoResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called GetJackpotValues"); return OK; }
    virtual StatusCode UpdateJackpotValues(UpdateJackpotValuesRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called UpdateJackpotValues"); return OK; }
    virtual StatusCode TriggerJackpot(TriggerJackpotRequest& request, TriggerJackpotResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called TriggerJackpot"); return OK; }
    virtual StatusCode ClaimJackpot(ClaimJackpotRequest& request, LevelInfoResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called ClaimJackpot"); return OK; }
    virtual StatusCode SetJackpotLevelWagers(LevelWagerRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called SetJackpotLevelWagers"); return OK; }
    virtual StatusCode CheckMysteryJackpot(CheckMysteryJackpotRequest& request, CheckMysteryJackpotResponse& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called CheckMysteryJackpot"); return OK; }
    virtual StatusCode SelectDenomination(SelectDenominationRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "GameServiceCallback from game", "Called SelectDenomination"); return OK; }
};
