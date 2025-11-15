#pragma once

#include "namica/core/Base.h"
#include "namica/core/Memory.h"
#include "namica/events/Event.h"

namespace Namica
{

struct WindowConfig;

class NAMICA_API Window
{
public:
    using EventCallBackFn = std::function<void(Event&)>;

public:
    Window() = default;
    virtual ~Window() = default;

    /**
     * @brief 给窗口注册事件回调
     */
    virtual void setEventCallBackFn(EventCallBackFn _eventCallBackFn) = 0;

    /**
     * @brief 返回当前窗口的宽度
     */
    virtual uint32_t getWidth() const noexcept = 0;

    /**
     * @brief 返回当前窗口的长度
     */
    virtual uint32_t getHeight() const noexcept = 0;

    /**
     * @brief 设置垂直同步
     */
    virtual void setVSync(bool _enable) = 0;

    /**
     * @brief 获取是否垂直同步
     */
    virtual bool getVSync() const noexcept = 0;

    /**
     * @brief 窗口轮询派发事件
     */
    virtual void pollEvents() = 0;

    /**
     * @brief 窗口中像素缓冲区交换
     */
    virtual void swapBuffers() = 0;

public:
    /**
     * @brief 创建Window对象
     */
    static Ref<Window> create(WindowConfig const& _windowConfig);
};

}  // namespace Namica