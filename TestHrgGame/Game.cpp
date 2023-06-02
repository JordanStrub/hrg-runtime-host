#include "pch.h"
#include "Game.h"
#include "TestHrgGameDlg.h"
#include "CommPlugin.h"

Game::Game(LogCallback* pLog)
    : _pLog((pLog))
{
}

void Game::UpdateParameters(std::map<std::string, std::string>& parameters)
{
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
}

void Game::UpdateDenom(long cents)
{
    _denomCents = cents;
    auto str = CurrencyString(cents);
    CTestHrgGameDlg::Instance()->UpdateDenomMeter(str);
}

void Game::UpdateBalance(long cents)
{
    auto str = CurrencyString(cents);
    CTestHrgGameDlg::Instance()->UpdateCreditMeter(str);
}

void Game::StartGame()
{
    _startThread = std::thread([=]
        {
            CommPlugin::Instance()->Start();
        });
}

void Game::EndGame()
{
    CommPlugin::Instance()->Stop();
}

std::string Game::CurrencyString(long cents)
{
    auto dollars = cents / 100;
    cents -= 100 * dollars;
    char buf[1024];
    sprintf(buf, "$%ld.%02ld", dollars, cents);
    _pLog->Log(LogDebug, "currency", buf);
    return std::string(buf);
}
