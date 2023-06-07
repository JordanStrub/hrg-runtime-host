#pragma once

#include "LogCallback.h"
#include "Server.h"
#include "NamedPipeServerTransport.h"
#include "NamedPipeClientTransport.h"
#include "RuntimeServiceCallback.h"
#include "RuntimePresentationServiceCallback.h"
#include "GameCallbacks.h"
#include "PresentationCallbacks.h"

class Game;

class CommPlugin
{
public:
    CommPlugin(LogCallback* pLogCallback, Game* pGame);
    static CommPlugin* Instance();
    ~CommPlugin();
    void Start();
    void Stop();
    void CheckStatus();
    void UpdateBet(uint64_t bet, uint64_t denom);
    void GameReady();
    void GameRoundEvent(GameRoundEventType type, GameRoundEventStage stage, GameRoundPlayMode mode, uint64_t bet = 0, uint64_t win = 0);
    void RuntimeEvent(RuntimeEventNotification_RuntimeEvent evt);
    void RuntimeFlag(RuntimeFlag flag, bool value);
    void PlayGame(uint64_t bet, uint64_t denom);
    void EndGame(uint64_t win);
    uint32_t GetRandom(uint32_t range);

private:
    Game* _pGame;

    NamedPipeClientTransport* _pClientTransport;
    Channel* _pClientChannel;
    GameCallbacks* _pGameCallbacks;
    PresentationCallbacks* _pPresentationCallbacks;

    ServiceCallbacks* _pServiceCallbacks;
    RuntimeServiceCallback* _pRuntimeServiceCallbacks;
    RuntimePresentationServiceCallback* _pRuntimePresentationServiceCallbacks;
    NamedPipeServerTransport* _pServerTransport;
    Server* _pServer;
};
