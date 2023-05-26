#include "Runtime.h"

Runtime::Runtime(LogCallback* pLogCallback)
    : _pLog(pLogCallback)
    , _pendingShutdown(false)
{
}

bool Runtime::SetConfigParameter(const char* key, const char* val)
{
    std::string first(key);
    std::string second(val);
    _configParameters[first] = second;
    return true;
}

bool Runtime::LoadFromModule(const char* psGameModule, const char* psLogFolder)
{
    ShellExecuteA(nullptr, "open", psGameModule, psLogFolder, nullptr, SW_SHOWDEFAULT);

    return true;
}

std::map<std::string, std::string>& Runtime::GetConfigParameters()
{
    return _configParameters;
}

void Runtime::Shutdown()
{
    _pendingShutdown = true;
}

bool Runtime::IsPendingShutdown()
{
    return _pendingShutdown;
}
