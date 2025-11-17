#pragma once

#include "namica/core/Base.h"
#include "namica/core/InputCodes.h"

#include <utility>

namespace Namica
{

class NAMICA_API Input
{
public:
    /// @brief 当前按键是否按下
    static bool isKeyPressed(KeyCode keycode);
    /// @brief 当前鼠标按键是否按下
    static bool isMouseButtonPressed(KeyCode button);
    /// @brief 获取当前鼠标坐标
    static std::pair<float, float> getMousePosition();
    /// @brief 获取当前鼠标X坐标
    static float getMouseX();
    /// @brief 获取当前鼠标Y坐标
    static float getMouseY();
};

}  // namespace Namica