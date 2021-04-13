#pragma once
#include "pch.h"

#include <time.h>
#include <fstream>
#include <sstream>
#include <mutex>

void OutputApiError(const char* apiName);
void OutputApiError(const char* func, const char* apiName);

class DebugLog
{
public:
    enum class Mode
    {
        None = 0,
        File = 1,
        UnityLog = 2,
    };

    using DebugLogFuncPtr = void(__stdcall*)(const char*);

    static void SetMode(Mode mode) { _mode = mode; }
    static void Create(std::string fileName)
    {
        if (_mode == Mode::File)
        {
            _fs.open(fileName);
            DebugLog::Log("Debug Log Start.");
        }
    }
    static void Destroy()
    {
        if (_mode == Mode::File)
        {
            DebugLog::Log("Debug Log End.");
            _fs.close();
        }
    }
    static void SetLogFunc(DebugLogFuncPtr func) { _logFunc = func; }
    static void SetErrorFunc(DebugLogFuncPtr func) { _errFunc = func; }

private:
    enum class Level
    {
        Log,
        Error
    };

    template <class T>
    static void Output(T&& arg)
    {
        if (_mode == Mode::None) return;
        if (_ss.good())
        {
            _ss << std::forward<T>(arg);
        }
    }

    static void Flush(Level level)
    {
        switch (_mode)
        {
        case Mode::None:
        {
            return;
        }
        case Mode::File:
        {
            if (_fs.good() && _ss.good())
            {
                const auto str = _ss.str();
                _fs << str << std::endl;
                _fs.flush();
            }
            break;
        }
        case Mode::UnityLog:
        {
            if (_ss.good())
            {
                switch (level)
                {
                case Level::Log:
                    if (_logFunc) _logFunc(_ss.str().c_str());
                    break;
                case Level::Error:
                    if (_errFunc) _errFunc(_ss.str().c_str());
                    break;
                }
            }
            break;
        }
        }
        _ss.str("");
        _ss.clear(std::stringstream::goodbit);
    }

    template <class Arg, class... RestArgs>
    static void _Log(Level level, Arg&& arg, RestArgs&&... restArgs)
    {
        Output(std::forward<Arg>(arg));
        _Log(level, std::forward<RestArgs>(restArgs)...);
    }

    static void _Log(Level level)
    {
        Flush(level);
    }

    static void OutputTime()
    {
        auto t = time(nullptr);
        tm tm;
        localtime_s(&tm, &t);
        char buf[64];
        strftime(buf, 64, "%F %T", &tm);
        Output("[");;
        Output(buf);
        Output("]");
    }

public:
    template <class Arg, class... RestArgs>
    static void Log(Arg&& arg, RestArgs&&... restArgs)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        Output("[Log]");
        OutputTime();
        Output(" ");
        _Log(Level::Log, std::forward<Arg>(arg), std::forward<RestArgs>(restArgs)...);
    }

    template <class Arg, class... RestArgs>
    static void Error(Arg&& arg, RestArgs&&... restArgs)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        Output("[Err]");
        OutputTime();
        Output(" ");
        _Log(Level::Error, std::forward<Arg>(arg), std::forward<RestArgs>(restArgs)...);
    }

private:
    static Mode _mode;
    static std::ofstream _fs;
    static std::ostringstream _ss;
    static DebugLogFuncPtr _logFunc;
    static DebugLogFuncPtr _errFunc;
    static std::mutex _mutex;
};

#ifdef _DEBUG
#define FUNCTION_SCOPE_TIMER \
    ScopedTimer _timer_##__COUNTER__([](std::chrono::microseconds us) \
    { \
        DebugLog::Log(__FUNCTION__, "@", __FILE__, ":", __LINE__, " => ", us.count(), " [us]"); \
    });
#define SCOPE_TIMER(Name) \
    ScopedTimer _timer_##__COUNTER__([](std::chrono::microseconds us) \
    { \
        DebugLog::Log(#Name, " => ", us.count(), " [us]"); \
    });
#else
#define FUNCTION_SCOPE_TIMER
#define SCOPE_TIMER(Name)
#endif