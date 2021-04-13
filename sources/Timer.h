#pragma once

#include <chrono>
#include <functional>

class ScopedReleaser
{
public:
    using ReleaseFuncType = std::function<void()>;
    ScopedReleaser(ReleaseFuncType&& func) : func_(func) {}
    ~ScopedReleaser() { func_(); }

private:
    const ReleaseFuncType func_;
};

class ScopedTimer
{
public:
    using microseconds = std::chrono::microseconds;
    using TimerFuncType = std::function<void(microseconds)>;
    ScopedTimer(TimerFuncType&& func)
        : func_(func)
        , start_(std::chrono::high_resolution_clock::now())
    {
    }
    ~ScopedTimer()
    {
        const auto end = std::chrono::high_resolution_clock::now();
        const auto time = std::chrono::duration_cast<microseconds>(end - start_);
        func_(time);
    }

private:
    const TimerFuncType func_;
    const std::chrono::time_point<std::chrono::steady_clock> start_;
};