#pragma once

#include <concepts>

#include "namica/Common.h"

namespace namica
{

template <typename T, Int D>
class Vector
{
public:
    Vector() noexcept = default;

    template <typename... Args>
        requires(sizeof...(Args) == D && (std::convertible_to<Args, T> && ...))
    explicit Vector(Args&&... args) noexcept;

    template <Int D2>
        requires(D2 > D)
    explicit Vector(Vector<T, D2> const& _other) noexcept;

    explicit Vector(T value) noexcept;

    [[nodiscard]] T& operator[](Int _index) noexcept;
    [[nodiscard]] T const& operator[](Int _index) const noexcept;

    // 特化访问方法
    [[nodiscard]] T& x() noexcept
        requires(D >= 1);
    [[nodiscard]] T const& x() const noexcept
        requires(D >= 1);
    [[nodiscard]] T& y() noexcept
        requires(D >= 2);
    [[nodiscard]] T const& y() const noexcept
        requires(D >= 2);
    [[nodiscard]] T& z() noexcept
        requires(D >= 3);
    [[nodiscard]] T const& z() const noexcept
        requires(D >= 3);
    [[nodiscard]] T& w() noexcept
        requires(D >= 4);
    [[nodiscard]] T const& w() const noexcept
        requires(D >= 4);
    [[nodiscard]] T& r() noexcept
        requires(D >= 1);
    [[nodiscard]] T const& r() const noexcept
        requires(D >= 1);
    [[nodiscard]] T& g() noexcept
        requires(D >= 2);
    [[nodiscard]] T const& g() const noexcept
        requires(D >= 2);
    [[nodiscard]] T& b() noexcept
        requires(D >= 3);
    [[nodiscard]] T const& b() const noexcept
        requires(D >= 3);
    [[nodiscard]] T& a() noexcept
        requires(D >= 4);

    [[nodiscard]] T const& a() const noexcept
        requires(D >= 4);

    // 整数类型的比较
    [[nodiscard]] Bool isEqual(Vector<T, D> const& _other) const noexcept
        requires std::integral<T>;

    // 浮点类型的比较
    [[nodiscard]] Bool isEqual(Vector<T, D> const& _other,
                               T const _tol = static_cast<T>(NAMICA_TOL)) const
        requires std::floating_point<T>;

    // 获取原始数据

    [[nodiscard]] T* data() noexcept;
    [[nodiscard]] T const* data() const noexcept;

    // 基础运算

    // 诸分量相乘
    [[nodiscard]] Vector operator*(Vector const& _other) const noexcept;
    [[nodiscard]] Vector operator*(Float const _val) const noexcept;
    Vector& operator*=(Float const _val) noexcept;
    [[nodiscard]] Vector operator+(Vector const& _other) const noexcept;
    Vector& operator+=(Vector const& _other) noexcept;
    [[nodiscard]] Vector operator-(Vector const& _other) const noexcept;
    Vector& operator-=(Vector const& _other) noexcept;

    // 点乘
    [[nodiscard]] Float dot(Vector const& _other) const noexcept;

    // 归一化操作
    Vector& normalize() noexcept
        requires std::floating_point<T>;
    [[nodiscard]] Vector normalized() const noexcept
        requires std::floating_point<T>;

private:
    T m_data[D]{};
};

using Vec2 = Vector<Float, 2>;
using Vec3 = Vector<Float, 3>;
using Vec4 = Vector<Float, 4>;

using Veci2 = Vector<Int, 2>;

}  // namespace namica

#include "namica/math/Vector.inl"