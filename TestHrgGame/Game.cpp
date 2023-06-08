#include "pch.h"
#include "Game.h"
#include "TestHrgGameDlg.h"
#include "CommPlugin.h"

Game::Game(LogCallback* pLog)
    : _pLog((pLog))
    , _denomCents(1)
    , _bankCents(0)
    , _winCents(0)
    , _betCents(0)
{
}

void Game::UpdateParameters(std::map<std::string, std::string>& parameters)
{
    _pLog->Log(LogDebug, "Game", "UpdateParameters");
    CTestHrgGameDlg::Instance()->UpdateParameters(parameters);

    auto pointer = parameters.find("/Runtime/Denomination");
    if (pointer != parameters.end())
    {
        UpdateDenom(std::stol(pointer->second));

        pointer = parameters.find("/Runtime/Account&balance");
        if (pointer != parameters.end())
        {
            UpdateBalance(std::stol(pointer->second));
        }
    }
    _pLog->Log(LogDebug, "Game", "UpdateParameters complete");
}

void Game::UpdateDenom(uint64_t cents)
{
    _pLog->Log(LogDebug, "Game", "UpdateDenom");
    _denomCents = cents;
    auto str = CurrencyString(cents);
    CTestHrgGameDlg::Instance()->UpdateDenomMeter(str);
    _pLog->Log(LogDebug, "Game", "UpdateDenom complete");
}

void Game::UpdateBalance(uint64_t cents)
{
    _pLog->Log(LogDebug, "Game", "UpdateBalance");
    _bankCents = cents;
    CTestHrgGameDlg::Instance()->UpdateCreditMeter(CurrencyAndCreditsString(cents));
    _pLog->Log(LogDebug, "Game", "UpdateBalance complete");
}

void Game::UpdateWin(uint64_t cents)
{
    _pLog->Log(LogDebug, "Game", "UpdateWin");
    _winCents = cents;
    CTestHrgGameDlg::Instance()->UpdateWinMeter(CurrencyAndCreditsString(cents));
    _pLog->Log(LogDebug, "Game", "UpdateWin Complete");
}

void Game::UpdatePlatformMessage(std::vector<std::string> messages)
{
    _pLog->Log(LogDebug, "Game", "UpdatePlatformMessage");
    CTestHrgGameDlg::Instance()->UpdatePlatformMessage(messages);
    _pLog->Log(LogDebug, "Game", "UpdatePlatformMessage Complete");
}

void Game::Start()
{
    _pLog->Log(LogDebug, "Game", "Start");
    UpdateWin(0);

    std::vector<std::string> options;
    for (auto bet = 1; bet <= 5; bet++)
    {
        auto betCents = _denomCents * bet;
        options.push_back(CurrencyAndCreditsString(betCents));
    }
    CTestHrgGameDlg::Instance()->UpdateBetChoices(options);

    _thread = std::thread([=]
        {
            CommPlugin::Instance()->Start();
            Idle();
        });
    _pLog->Log(LogDebug, "Game", "Start complete");
}

void Game::Stop()
{
    _pLog->Log(LogDebug, "Game", "Stop");
    CommPlugin::Instance()->Stop();
    _pLog->Log(LogDebug, "Game", "Stop complete");
}

void Game::Idle()
{
    _pLog->Log(LogDebug, "Game", "Idle");
    GameRoundEvent(GameRoundEventType::Idle, Invoked);

    CommPlugin::Instance()->GameReady();

    CTestHrgGameDlg::Instance()->UpdateStatusLine("Idle");
    _pLog->Log(LogDebug, "Game", "Idle complete");
}

bool Game::TrySetBet(std::string betCredits)
{
    _pLog->Log(LogDebug, "Game", "TrySetBet");
    auto parenPos = betCredits.find_first_of("(") + 1;
    auto blankPos = betCredits.find_last_of(" ");
    if (parenPos * blankPos == 0)
    {
        return false;
    }

    auto betCents = std::stol(betCredits.substr(parenPos, blankPos - parenPos)) * _denomCents;
    if (betCents > _bankCents)
    {
        // Can't afford this bet
        return false;
    }

    _betCents = betCents;
    CommPlugin::Instance()->UpdateBet(betCents, _denomCents);
    _pLog->Log(LogDebug, "Game", "TrySetBet complete");
    return true;
}

void Game::GameRoundEvent(GameRoundEventType type, GameRoundEventStage stage)
{
    CommPlugin::Instance()->GameRoundEvent(type, stage, Mode_Normal, _betCents, _winCents);
}

void Game::PlayGame()
{
    _pLog->Log(LogDebug, "Game", "PlayGame");

    UpdateWin(0);

    CommPlugin::Instance()->PlayGame(_betCents, _denomCents);

    _pLog->Log(LogDebug, "Game", "PlayGame complete");
}

void Game::HandlePlayGameResponse()
{
    _thread.join();
    _thread = std::thread([=]
        {
            _pLog->Log(LogDebug, "Game", "HandlePlayGameResponse");
            GameRoundEvent(Primary, Begin);

            CommPlugin::Instance()->RuntimeFlag(AllowGameRound, false);

            GetInitialRandoms(5, 50); // just some typical calls, we don't use the results

            GameRoundEvent(Present, Begin);

            uint32_t winCredits = GetRandom(2) == 0 ? 0 : GetRandom(40);
            _winCents = winCredits * _denomCents;

            GameRoundEvent(Primary, Invoked);

            _pLog->Log(LogDebug, "Game", "HandlePlayGameResponse complete");
        });
}

void Game::EndGame()
{
    _pLog->Log(LogDebug, "Game", "EndGame");

    GameRoundEvent(GameResults, Invoked);

    GameRoundEvent(Primary, Completed);

    CommPlugin::Instance()->EndGame(_winCents);

    GameRoundEvent(Present, Invoked);

    CTestHrgGameDlg::Instance()->UpdateWinMeter(CurrencyAndCreditsString(_winCents));
    _pLog->Log(LogDebug, "Game", "EndGame complete");
}

uint32_t Game::GetRandom(uint32_t range)
{
    _pLog->Log(LogDebug, "Game", "GetRandom");
    auto random = CommPlugin::Instance()->GetRandom(range);
    _pLog->Log(LogDebug, "Game", "GetRandom complete");
    return random;
}

void Game::GetInitialRandoms(int count, uint32_t range)
{
    _pLog->Log(LogDebug, "Game", "GetRandoms");
    for (auto i = 0; i < count; i++)
    {
        GetRandom(range);
    }
    _pLog->Log(LogDebug, "Game", "GetRandoms complete");
}

std::string Game::CurrencyString(uint64_t cents)
{
    auto dollars = cents / 100;
    cents -= 100 * dollars;
    char buf[1024];
    sprintf(buf, "$%llu.%02llu", dollars, cents);
    _pLog->Log(LogDebug, "currency", buf);
    return std::string(buf);
}

std::string Game::CurrencyAndCreditsString(uint64_t cents)
{
    auto credits = cents / _denomCents;
    auto dollars = cents / 100;
    cents -= 100 * dollars;
    char buf[1024];
    sprintf(buf, "$%llu.%02llu (%llu cr)", dollars, cents, credits);
    _pLog->Log(LogDebug, "currency+credits", buf);
    return std::string(buf);
}
