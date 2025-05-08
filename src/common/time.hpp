#pragma once

#include <chrono>

#define DEFAULT_MAX_FPS 144

namespace namica
{
class Application;

class Time
{
    friend class Application;

public:
    static Time& getInstance()
    {
        static Time instance;
        return instance;
    }

public:
    /// @brief 设置每秒跑多少帧
    void setFPS(int fps) noexcept
    {
        this->fps = fps;
        frame_duration = std::chrono::nanoseconds{1'000'000'000 / fps};
    }

    /// @brief 获取当前应用的最大帧率
    int getMaxFPS() const noexcept
    {
        return fps;
    }

    /// @brief 获取当前应用程序的帧率
    int getCurrentFPS() const noexcept
    {
        return static_cast<int>(1 / delta);
    }

    /// @brief 获取当前的时间比例
    float getScale() const noexcept
    {
        return scale;
    }

    /// @brief 设置当前的时间比例
    void setScale(float scale) noexcept
    {
        this->scale = scale;
    }

    /// @brief 获取当前的帧间隔(乘以时间比例)
    float getDelta() const noexcept
    {
        return delta * scale;
    }

private:
    // 引擎内部私有方法, 不可随意访问
    // 控制每帧间隔时间

    /// @brief 进入循环开始前的计时
    void startTick()
    {
        last_tick = std::chrono::steady_clock::now();
    }

    /// @brief 计算当前帧和上帧之间的delta
    void frameTick()
    {
        auto frame_start = std::chrono::steady_clock::steady_clock::now();
        delta = std::chrono::duration<float>{frame_start - last_tick}.count();
        last_tick = frame_start;
    }

    /// @brief 返回依据上次frameTick后，剩余的当前帧持续时间,可以用作当前CPU时间片放出,
    /// 减少cpu的占用
    std::chrono::nanoseconds sleepDuration()
    {
        return frame_duration - (std::chrono::steady_clock::steady_clock::now() - last_tick);
    }

private:
    std::chrono::nanoseconds frame_duration;          // 每帧之间应该持续的时间
    std::chrono::steady_clock::time_point last_tick;  // 保存当前状态的上一次记录tick

    int fps;
    float scale;
    float delta;

private:
    Time()
    {
        setFPS(DEFAULT_MAX_FPS);
        scale = 1.0f;  // 时间比例
        delta = 1.0f / DEFAULT_MAX_FPS;
    }
    ~Time() = default;
    Time(const Time&) = delete;
    void operator=(const Time&) = delete;
};
}  // namespace namica