#pragma once
class Runtime
{
    bool _pendingShutdown;

public:
    bool SetParameter(const char*, const char*, const char*)
    {
        return true;
    }

    bool LoadFromModule(const char* psGameModule)
    {
        return true;
    }

    bool Configure()
    {
        return true;
    }

    bool Initialize()
    {
        return true;
    }

    void Shutdown()
    {
        _pendingShutdown = true;
    }

    bool IsPendingShutdown()
    {
        return _pendingShutdown;
    }
};
