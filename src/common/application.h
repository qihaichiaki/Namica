#pragma once

// application 应用框架模板

namespace namica
{

class Application
{
public:
    [[nodiscard]] Application() = default;
    virtual ~Application() = default;

public:
    /// @brief 应用程序的初始化, 通常用来初始化各个系统, 设置各个管理器，以及相关窗口初始化等
    virtual bool init() = 0;

    /// @brief 当前帧前处理
    virtual void beginFrame() {}

    /// @brief 事件处理
    virtual void processEvents() = 0;

    /// @brief 逻辑更新
    virtual void update() = 0;

    /// @brief 画面渲染
    virtual void render() = 0;

    /// @brief 当前帧后处理
    virtual void endFrame() = 0;

public:
    /// @brief 运行应用程序
    void run();
};
}  // namespace namica