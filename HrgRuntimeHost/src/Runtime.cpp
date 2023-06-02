#include "Runtime.h"

Runtime::Runtime(LogCallback* pLogCallback)
    : _pLog(pLogCallback)
    , _pendingShutdown(false)
{
}

bool Runtime::LoadFromModule(const char* psGameModule, const char* psLogFolder)
{
    ShellExecuteA(nullptr, "open", psGameModule, psLogFolder, nullptr, SW_SHOWDEFAULT);

    return true;
}

void Runtime::Shutdown()
{
    _pendingShutdown = true;
}

bool Runtime::IsPendingShutdown()
{
    return _pendingShutdown;
}
