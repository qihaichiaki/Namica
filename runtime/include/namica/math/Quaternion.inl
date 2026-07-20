#pragma once

#include "namica/math/Quaternion.h"
#include <cmath>

namespace namica
{

inline Quat::Quat() noexcept
{
}

inline Quat::Quat(Float _w, Float _x, Float _y, Float _z) noexcept : w{_w}, x{_x}, y{_y}, z{_z}
{
}

inline Float Quat::lengthSquared() const noexcept
{
    return w * w + x * x + y * y + z * z;
}

inline Float Quat::length() const noexcept
{
    return std::sqrt(this->lengthSquared());
}

inline Quat& Quat::normalize() noexcept
{
    Quat& result{*this};
    Float const len{this->length()};

    if (len != 0.0f)
    {
        Float const inverseLength{1.0f / len};
        result.w *= inverseLength;
        result.x *= inverseLength;
        result.y *= inverseLength;
        result.z *= inverseLength;
    }

    return result;
}

inline Quat Quat::normalized() const noexcept
{
    Quat result{*this};
    return result.normalize();
}

inline Quat Quat::operator*(Quat const& _other) const noexcept
{
    return Quat{w * _other.w - x * _other.x - y * _other.y - z * _other.z,
                w * _other.x + x * _other.w + y * _other.z - z * _other.y,
                w * _other.y + y * _other.w + z * _other.x - x * _other.z,
                w * _other.z + z * _other.w + x * _other.y - y * _other.x};
}

inline Vec3 Quat::operator*(Vec3 const& _vector) const noexcept
{
    Vec3 const quaternionVector{x, y, z};
    Vec3 const uv{quaternionVector.y() * _vector.z() - quaternionVector.z() * _vector.y(),
                  quaternionVector.z() * _vector.x() - quaternionVector.x() * _vector.z(),
                  quaternionVector.x() * _vector.y() - quaternionVector.y() * _vector.x()};
    Vec3 const uuv{quaternionVector.y() * uv.z() - quaternionVector.z() * uv.y(),
                   quaternionVector.z() * uv.x() - quaternionVector.x() * uv.z(),
                   quaternionVector.x() * uv.y() - quaternionVector.y() * uv.x()};
    return _vector + (uv * w + uuv) * 2.0f;
}

inline Mat4 Quat::toMatrix() const noexcept
{
    Mat4 result{1.0f};

    Float const xx{x * x};
    Float const yy{y * y};
    Float const zz{z * z};

    Float const xy{x * y};
    Float const xz{x * z};
    Float const yz{y * z};

    Float const wx{w * x};
    Float const wy{w * y};
    Float const wz{w * z};

    // Mat4[column][row]
    result[0][0] = 1.0f - 2.0f * (yy + zz);
    result[0][1] = 2.0f * (xy + wz);
    result[0][2] = 2.0f * (xz - wy);

    result[1][0] = 2.0f * (xy - wz);
    result[1][1] = 1.0f - 2.0f * (xx + zz);
    result[1][2] = 2.0f * (yz + wx);

    result[2][0] = 2.0f * (xz + wy);
    result[2][1] = 2.0f * (yz - wx);
    result[2][2] = 1.0f - 2.0f * (xx + yy);

    return result;
}

inline Quat Quat::angleAxis(float const _angleRadians, Vec3 const& _normalizedAxis) noexcept
{
    Float const halfAngle{_angleRadians * 0.5f};
    Float const sinHalfAngle{std::sin(halfAngle)};

    return Quat{std::cos(halfAngle),
                _normalizedAxis.x() * sinHalfAngle,
                _normalizedAxis.y() * sinHalfAngle,
                _normalizedAxis.z() * sinHalfAngle};
}

inline Quat Quat::fromEuler(Vec3 const& _eulerRadians) noexcept
{
    Float const halfX{_eulerRadians.x() * 0.5f};
    Float const halfY{_eulerRadians.y() * 0.5f};
    Float const halfZ{_eulerRadians.z() * 0.5f};

    Float const cx{std::cos(halfX)};
    Float const sx{std::sin(halfX)};

    Float const cy{std::cos(halfY)};
    Float const sy{std::sin(halfY)};

    Float const cz{std::cos(halfZ)};
    Float const sz{std::sin(halfZ)};

    return Quat{cx * cy * cz + sx * sy * sz,
                sx * cy * cz - cx * sy * sz,
                cx * sy * cz + sx * cy * sz,
                cx * cy * sz - sx * sy * cz};
}

}  // namespace namica