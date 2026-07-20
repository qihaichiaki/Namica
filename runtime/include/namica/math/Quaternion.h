#pragma once

#include "namica/Common.h"
#include "namica/math/Matrix.h"
#include "namica/math/Vector.h"

namespace namica
{

// 四元数表示绕轴旋转的信息
struct Quat
{
    Float w{1.0f};
    Float x{0.0f};
    Float y{0.0f};
    Float z{0.0f};

    Quat() noexcept;
    Quat(Float _w, Float _x, Float _y, Float _z) noexcept;

    // 工具函数
    [[nodiscard]] Float lengthSquared() const noexcept;
    [[nodiscard]] Float length() const noexcept;

    Quat& normalize() noexcept;
    [[nodiscard]] Quat normalized() const noexcept;

    [[nodiscard]] Quat operator*(Quat const& _other) const noexcept;
    [[nodiscard]] Vec3 operator*(Vec3 const& _vector) const noexcept;

    // 实用函数
    [[nodiscard]] Mat4 toMatrix() const noexcept;
    [[nodiscard]] static Quat angleAxis(float const _angleRadians,
                                        Vec3 const& _normalizedAxis) noexcept;
    [[nodiscard]] static Quat fromEuler(Vec3 const& _eulerRadians) noexcept;
};

}  // namespace namica

#include "namica/math/Quaternion.inl"