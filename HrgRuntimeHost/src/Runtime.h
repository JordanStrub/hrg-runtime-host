#pragma once

#include "LogCallback.h"

class Runtime
{
    LogCallback* _pLog;
    bool _pendingShutdown;

public:
    Runtime(LogCallback* pLogCallback);
    bool SetParameter(const char*, const char*, const char*);
    bool LoadFromModule(const char* psGameModule);
    bool Configure();
    bool Initialize();
    void Shutdown();
    bool IsPendingShutdown();
};
