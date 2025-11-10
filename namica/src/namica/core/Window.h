#pragma once

#include "namica/core/Base.h"
#include "namica/core/Memory.h"

namespace Namica
{

struct WindowConfig;

class NAMICA_API Window
{
public:
    Window() = default;
    virtual ~Window() = default;

    /**
     * @brief 返回当前窗口的宽度
     */
    virtual unsigned int getWidth() const noexcept = 0;

    /**
     * @brief 返回当前窗口的长度
     */
    virtual unsigned int getHeight() const noexcept = 0;

    /**
     * @brief 设置垂直同步
     */
    virtual void setVSync(bool _enable) = 0;

    /**
     * @brief 获取是否垂直同步
     */
    virtual bool getVSync() const noexcept = 0;

public:
    /**
     * @brief 创建Window对象
     */
    static Ref<Window> create(WindowConfig const& _windowConfig);
};

}  // namespace Namica