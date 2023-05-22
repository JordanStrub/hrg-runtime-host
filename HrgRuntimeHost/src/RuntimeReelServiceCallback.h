#pragma once

#include "LogCallback.h"
#include "runtimeReelService.snapp.pb.h"
#include "snapp.pb.h"

using namespace Aristocrat::GdkRuntime::v1;
using namespace Aristocrat::Snapp;

class RuntimeReelServiceCallback : public IRuntimeReelServiceCallback
{
    LogCallback* _pLog;
public:
    RuntimeReelServiceCallback(LogCallback* pLogCallback) : _pLog(pLogCallback) {}
    virtual StatusCode UpdateReelState(UpdateReelStateRequest& request, Empty& response, Status& status) { _pLog->Log(LogInfo, "RuntimeReelServiceCallback", "Called UpdateReelState"); return OK; }
};
