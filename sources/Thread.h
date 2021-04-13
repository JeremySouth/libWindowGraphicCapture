#pragma once

#include <functional>
#include <chrono>
#include <thread>
#include <atomic>

#include "Timer.h"

class ScopedThreadSleeper : public ScopedTimer
{
public:
    template <class T>
    explicit ScopedThreadSleeper(const T& duration) :
        ScopedTimer([duration](microseconds us)
            {
                const auto waitTime = duration - us;
                if (waitTime > microseconds::zero())
                {
                    std::this_thread::sleep_for(waitTime);
                }
            })
    {}
};

class ThreadLoop
{
public:
    using ThreadFunc = std::function<void()>;
    using microseconds = std::chrono::microseconds;

    ThreadLoop() {}
    ~ThreadLoop() { Stop(); }
    void Start(
        const ThreadFunc& func,
        const microseconds& interval = microseconds(1'000'000 / 60))
    {
        if (isRunning_) return;

        func_ = func;
        interval_ = interval;
        isRunning_ = true;

        if (thread_.joinable())
        {
            DebugLog::Error(__FUNCTION__, " => Thread is running");
            thread_.join();
        }

        thread_ = std::thread([this]
            {
                while (isRunning_)
                {
                    ScopedThreadSleeper sleeper(interval_);
                    func_();
                }
            });
    }
    void Restart();
    void Stop()
    {
        if (!isRunning_) return;

        isRunning_ = false;

        if (thread_.joinable())
        {
            thread_.join();
        }
    }
    bool IsRunning() const;
    bool HasFunction() const;

private:
    std::thread thread_;
    std::atomic<bool> isRunning_ = false;
    microseconds interval_ = microseconds::zero();
    ThreadFunc func_ = nullptr;
};