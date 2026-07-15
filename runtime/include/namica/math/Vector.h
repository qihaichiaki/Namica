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

    explicit Vector(T value) noexcept;

    T& operator[](Int _index) noexcept;
    T const& operator[](Int _index) const noexcept;

    // 特化访问方法
    T& x() noexcept
        requires(D >= 1);
    T const& x() const noexcept
        requires(D >= 1);
    T& y() noexcept
        requires(D >= 2);
    T const& y() const noexcept
        requires(D >= 2);
    T& z() noexcept
        requires(D >= 3);
    T const& z() const noexcept
        requires(D >= 3);
    T& w() noexcept
        requires(D >= 4);
    T const& w() const noexcept
        requires(D >= 4);
    T& r() noexcept
        requires(D >= 1);
    T const& r() const noexcept
        requires(D >= 1);
    T& g() noexcept
        requires(D >= 2);
    T const& g() const noexcept
        requires(D >= 2);
    T& b() noexcept
        requires(D >= 3);
    T const& b() const noexcept
        requires(D >= 3);
    T& a() noexcept
        requires(D >= 4);

    T const& a() const noexcept
        requires(D >= 4);

    // 整数类型的比较
    [[nodiscard]] Bool isEqual(Vector<T, D> const& _other) const noexcept
        requires std::integral<T>;

    // 浮点类型的比较
    [[nodiscard]] Bool isEqual(Vector<T, D> const& _other,
                               T const _tol = static_cast<T>(NAMICA_TOL)) const
        requires std::floating_point<T>;

    // 获取原始数据
    T* data() noexcept;
    T const* data() const noexcept;

    // 基础运算
    Vector operator*(Vector const& _other) const noexcept;
    Vector operator*(Float const _val) const noexcept;
    Vector operator+(Vector const& _other) const noexcept;

private:
    T m_data[D]{};
};

using Vec2 = Vector<Float, 2>;
using Vec3 = Vector<Float, 3>;
using Vec4 = Vector<Float, 4>;

using Veci2 = Vector<Int, 2>;

}  // namespace namica

#include "namica/math/Vector.inl"