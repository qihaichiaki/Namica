#pragma once

#include <cstdint>

namespace namica
{

using Int8 = std::int8_t;
using Int32 = std::int32_t;
using UInt8 = std::int8_t;
using UInt32 = std::uint32_t;

using Int = Int32;
using UInt = UInt32;
using Float = float;
using Double = double;
using Bool = bool;

[[maybe_unused]] static constexpr Bool True{true};
[[maybe_unused]] static constexpr Bool False{false};

// 魔法数字
[[maybe_unused]] static constexpr Float NAMICA_TOL{1e-6f};  // 引擎容差

}  // namespace namica