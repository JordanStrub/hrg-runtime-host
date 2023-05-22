#pragma once

#include "LogCallback.h"
//#include "IExternalLogger.h"
#include "Server.h"

using namespace Aristocrat::Snapp;

class CommPlugin
{
public:
    CommPlugin(LogCallback* _pLogCallback);
    void Start();
    void Stop();
    void Update(double elapsedTime);

private:
    Server* _server;
};
