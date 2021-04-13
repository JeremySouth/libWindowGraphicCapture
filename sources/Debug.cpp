#include "pch.h"
#include "Debug.h"

decltype(DebugLog::_mode)    DebugLog::_mode = DebugLog::Mode::File;
decltype(DebugLog::_logFunc) DebugLog::_logFunc = nullptr;
decltype(DebugLog::_errFunc) DebugLog::_errFunc = nullptr;
decltype(DebugLog::_fs)      DebugLog::_fs;
decltype(DebugLog::_ss)      DebugLog::_ss;
decltype(DebugLog::_mutex)   DebugLog::_mutex;

void OutputApiError(const char* apiName)
{
    const auto error = ::GetLastError();
    DebugLog::Error(apiName, "() failed with error code: ", error);
}
void OutputApiError(const char* func, const char* apiName)
{
    const auto error = ::GetLastError();
    DebugLog::Error(func, "() => ", apiName, "() failed with error code: ", error);
}