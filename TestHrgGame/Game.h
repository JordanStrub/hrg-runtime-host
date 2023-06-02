#pragma once
#include "LogCallback.h"
#include <map>
#include <string>
#include <thread>

class Game
{
public:
    Game(LogCallback* pLog);

    void UpdateParameters(std::map<std::string, std::string>& parameters);
    void UpdateDenom(long cents);
    void UpdateBalance(long cents);

    void StartGame();
    void EndGame();

private:
    std::string CurrencyString(long credits);
    std::thread _startThread; // < Thread to start on.
    LogCallback* _pLog;
    long _denomCents;
};

