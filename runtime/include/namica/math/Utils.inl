#pragma once

#include "namica/math/Utils.h"
#include "namica/math/Constants.h"

namespace namica
{

inline constexpr Float radians(Float const _degrees) noexcept
{
    return _degrees * Constants::PI / 180.0f;
}

inline constexpr Float degrees(Float const _radians) noexcept
{
    return _radians * 180.0f / Constants::PI;
}

}  // namespace namica