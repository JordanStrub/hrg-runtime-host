#include "Runtime.h"

Runtime::Runtime(LogCallback* pLogCallback)
    : _pLog(pLogCallback)
{
}

bool Runtime::SetParameter(const char*, const char*, const char*)
{
    return true;
}

bool Runtime::LoadFromModule(const char* psGameModule)
{
    return true;
}

bool Runtime::Configure()
{
    return true;
}

bool Runtime::Initialize()
{
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
