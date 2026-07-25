#pragma once

#include "namica/Common.h"

namespace namica
{

/**
 * @brief 将角度转换为弧度
 *
 * @param _degree 输入角度
 * @return 输出弧度
 */
[[nodiscard]] constexpr Float radians(Float const _degrees) noexcept;

/**
 * @brief 将弧度转换为角度
 *
 * @param _radians 输入弧度
 * @return 输出角度
 */
[[nodiscard]] constexpr Float degrees(Float const _radians) noexcept;

}  // namespace namica

#include "namica/math/Utils.inl"