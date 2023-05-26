#pragma once

#include "LogCallback.h"
#include <string>
#include <map>

class Runtime
{
    LogCallback* _pLog;
    bool _pendingShutdown;
    std::map<std::string, std::string> _configParameters;

public:
    Runtime(LogCallback* pLogCallback);
    bool SetConfigParameter(const char* key, const char* val);
    bool LoadFromModule(const char* psGameModule, const char* psLogFolder);
    std::map<std::string, std::string>& GetConfigParameters();
    void Shutdown();
    bool IsPendingShutdown();
};
