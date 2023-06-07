#include "pch.h"
#include "CommPlugin.h"
#include "Server.h"
#include "Logger.h"
#include "Game.h"

using namespace Aristocrat::Snapp;
using namespace google::protobuf;

static LogCallback* pLog = nullptr;
static CommPlugin* commPlugin = nullptr;

const wchar_t* HostServerPipeName = L"HostGameSnapp";
const wchar_t* HostClientPipeName = L"HostRuntimeSnapp";

static void StaticLog(int level, std::string message)
{
    if (pLog)
    {
        int localLevel = LogPlugin;
        switch (level)
        {
        case Debug: localLevel |= LogDebug; break;
        case Info: localLevel |= LogInfo; break;
        case Warn: localLevel |= LogWarning; break;
        case Fatal: localLevel |= LogSystem; break;
        // case Error: gives some ambiguity error, so let it be default.
        default: localLevel |= LogError; break;
        }
        pLog->Log(localLevel, "Snapp", message.c_str());
    }
}

CommPlugin* CommPlugin::Instance()
{
    return commPlugin;
}

CommPlugin::CommPlugin(LogCallback* pLogCallback, Game* pGame)
    : _pGame(pGame)
{
    commPlugin = this;
    pLog = pLogCallback;
    SetExternalLogger(StaticLog);

    // Server
    _pServiceCallbacks = new ServiceCallbacks();
    _pRuntimeServiceCallbacks = new RuntimeServiceCallback(pLog, pGame);
    _pRuntimePresentationServiceCallbacks = new RuntimePresentationServiceCallback(pLog);
    _pServiceCallbacks->AddCallback(*_pRuntimeServiceCallbacks);
    _pServiceCallbacks->AddCallback(*_pRuntimePresentationServiceCallbacks);

    _pServerTransport = new NamedPipeServerTransport(HostServerPipeName);
    _pServer = new Server(_pServerTransport, _pServiceCallbacks);
    _pServer->Start();
    pLog->Log(LogInfo, "CommPlugin", "Started server comms");


    // Client
    _pClientTransport = new NamedPipeClientTransport(HostClientPipeName);
    pLog->Log(LogInfo, "CommPlugin", "Created client pipe transport");
    _pClientChannel = new Channel(_pClientTransport);
    pLog->Log(LogInfo, "CommPlugin", "Created client channel");

    _pGameCallbacks = new GameCallbacks(pLog, _pClientChannel);
    _pPresentationCallbacks = new PresentationCallbacks(_pGameCallbacks, pLog, _pClientChannel);
    pLog->Log(LogInfo, "CommPlugin", "Created client callbacks");

    _pClientChannel->Connect();
    pLog->Log(LogInfo, "CommPlugin", "Started client comms");
}

CommPlugin::~CommPlugin()
{
    delete _pPresentationCallbacks;
    delete _pGameCallbacks;
    delete _pClientChannel;
    delete _pClientTransport;

    pLog->Log(LogInfo, "CommPlugin", "Stopping server");
    _pServer->Stop();
    delete _pServer;
    delete _pServerTransport;
    delete _pRuntimePresentationServiceCallbacks;
    delete _pRuntimeServiceCallbacks;
    delete _pServiceCallbacks;
    pLog->Log(LogInfo, "CommPlugin", "Over and out");
}


void CommPlugin::Start()
{
    JoinRequest join;
    Empty empty;
    Status status;
    _pGameCallbacks->Join(join, empty, status);

    RuntimeEvent(RuntimeEventNotification::RequestConfiguration);

    RuntimeEvent(RuntimeEventNotification::NotifyGameReady);
    pLog->Log(LogInfo, "CommPlugin", "Started");
}

void CommPlugin::Stop()
{
    LeaveRequest leave;
    Empty empty;
    Status status;
    _pGameCallbacks->Leave(leave, empty, status);
    pLog->Log(LogInfo, "CommPlugin", "Stopped");
}

void CommPlugin::CheckStatus()
{
    
}

void CommPlugin::UpdateBet(uint64_t bet, uint64_t denom)
{
    pLog->Log(LogInfo, "CommPlugin", "UpdateBet");
    GameRoundEvent(Betting, Begin, Mode_Normal);

    // bet:credits=multiplier, denom:cents
    UpdateBetOptionsRequest request;
    request.set_betmultiplier(bet);
    request.set_linecost(1);
    request.set_stakeamount(bet * denom);
    request.set_wager(bet);
    Empty empty;
    Status status;
    _pGameCallbacks->UpdateBetOptions(request, empty, status);

    GameRoundEvent(Betting, Completed, Mode_Normal);
    pLog->Log(LogInfo, "CommPlugin", "UpdateBet complete");
}

void CommPlugin::GameReady()
{
    pLog->Log(LogInfo, "CommPlugin", "GameReady");
    RuntimeEventNotification request;
    request.set_runtimeevent(RuntimeEventNotification_RuntimeEvent_RequestAllowGameRound);
    Empty empty;
    Status status;
    _pGameCallbacks->RuntimeEvent(request, empty, status);

    pLog->Log(LogInfo, "CommPlugin", "GameReady complete");
}

void CommPlugin::GameRoundEvent(GameRoundEventType type, GameRoundEventStage stage, GameRoundPlayMode mode, uint64_t bet, uint64_t win)
{
    char buf[1024];
    sprintf(buf, "GameRoundEvent type=%d stage=%d bet=%llu win=%llu", type, stage, bet, win);
    pLog->Log(LogInfo, "CommPlugin", buf);
    GameRoundEventRequest request;
    request.set_type(type);
    request.set_stage(stage);
    request.set_playmode(mode);
    request.set_betamount(bet);
    request.set_winamount(win);
    Empty empty;
    Status status;
    _pGameCallbacks->GameRoundEvent(request, empty, status);
    sprintf(buf + strlen(buf), " complete");
    pLog->Log(LogInfo, "CommPlugin", buf);
}

void CommPlugin::RuntimeEvent(RuntimeEventNotification_RuntimeEvent evt)
{
    char buf[1024];
    sprintf(buf, "RuntimeEvent evt=%d", evt);
    pLog->Log(LogInfo, "CommPlugin", buf);
    RuntimeEventNotification request;
    request.set_runtimeevent(evt);
    Empty empty;
    Status status;
    _pGameCallbacks->RuntimeEvent(request, empty, status);
    sprintf(buf + strlen(buf), " complete");
    pLog->Log(LogInfo, "CommPlugin", buf);
}

void CommPlugin::RuntimeFlag(::RuntimeFlag flag, bool value)
{
    char buf[1024];
    sprintf(buf, "Runtime Flag flag=%d val=%d", flag, value);
    pLog->Log(LogInfo, "CommPlugin", buf);
    RuntimeFlagNotification request;
    request.set_flag(flag);
    request.set_state(value);
    Empty empty;
    Status status;
    _pGameCallbacks->RuntimeFlagChange(request, empty, status);
    sprintf(buf + strlen(buf), " complete");
    pLog->Log(LogInfo, "CommPlugin", buf);
}

void CommPlugin::PlayGame(uint64_t bet, uint64_t denom)
{
    pLog->Log(LogInfo, "CommPlugin", "PlayGame");
    BeginGameRoundAsyncRequest request;
    request.set_betamount(bet);
    request.set_denomination(denom);
    GameInfo info;
    info.set_betlinepreset(0);
    Any packedInfo;
    packedInfo.PackFrom(info);
    request.mutable_gamedetails()->Add();
    (*request.mutable_gamedetails())[0] = packedInfo;
    Empty empty;
    Status status;
    _pGameCallbacks->BeginGameRoundAsync(request, empty, status);

    pLog->Log(LogInfo, "CommPlugin", "PlayGame complete");
}

void CommPlugin::EndGame(uint64_t win)
{
    pLog->Log(LogInfo, "CommPlugin", "EndGame");

    EndGameRoundRequest request;
    request.set_winamount(win);
    Empty empty;
    Status status;
    _pGameCallbacks->EndGameRound(request, empty, status);
    pLog->Log(LogInfo, "CommPlugin", "EndGame complete");
}

uint32_t CommPlugin::GetRandom(uint32_t range)
{
    pLog->Log(LogInfo, "CommPlugin", "GetRandom");
    GetRandomNumber32Request request;
    request.set_range(range);
    GetRandomNumber32Response response;
    Status status;
    _pGameCallbacks->GetRandomNumber32(request, response, status);
    pLog->Log(LogInfo, "CommPlugin", "GetRandom complete");
    return response.value();
}
