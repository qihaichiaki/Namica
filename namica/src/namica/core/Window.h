#pragma once

#include "namica/core/Base.h"
#include "namica/core/Memory.h"

namespace Namica
{

struct WindowConfig;

class NAMICA_API Window
{
public:
    virtual ~Window() = default;

public:
    /**
     * @brief 创建Window对象
     */
    static Ref<Window> create(WindowConfig const& _windowConfig);

private:
    Window() = default;
};

}  // namespace Namica