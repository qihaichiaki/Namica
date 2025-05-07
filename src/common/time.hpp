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
    Time& getInstance()
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
    void startFrame() {}

private:
    std::chrono::nanoseconds frame_duration;  // 每帧之间应该持续的时间

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