#pragma once
#include "LogCallback.h"
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "GameCallbacks.h"

class Game
{
public:
    Game(LogCallback* pLog);

    void UpdateParameters(std::map<std::string, std::string>& parameters);
    void UpdateDenom(uint64_t cents);
    void UpdateBalance(uint64_t cents);
    void UpdateWin(uint64_t cents);
    void UpdatePlatformMessage(std::vector<std::string> messages);

    void Start();
    void Stop();
    void Idle();
    bool TrySetBet(std::string betCredits);
    void GameRoundEvent(GameRoundEventType type, GameRoundEventStage stage);
    void PlayGame();
    void HandlePlayGameResponse();
    void EndGame();
    uint32_t GetRandom(uint32_t range);
    void GetInitialRandoms(int count, uint32_t range);

private:
    std::string CurrencyString(uint64_t cents);
    std::string CurrencyAndCreditsString(uint64_t cents);

    std::thread _thread; // < Thread to start on.
    LogCallback* _pLog;

    uint64_t _denomCents;
    uint64_t _bankCents;
    uint64_t _winCents;
    uint64_t _betCents;
};

