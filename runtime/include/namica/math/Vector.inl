#pragma once

#include "namica/math/Vector.h"
#include <cmath>
#include <utility>

namespace namica
{

template <typename T, Int D>
template <typename... Args>
    requires(sizeof...(Args) == D && (std::convertible_to<Args, T> && ...))
inline Vector<T, D>::Vector(Args&&... args) noexcept
    : m_data{static_cast<T>((std::forward<Args>(args)))...}
{
}

template <typename T, Int D>
template <Int D2>
    requires(D2 > D)
inline Vector<T, D>::Vector(Vector<T, D2> const& _other) noexcept
{
    for (Int i{0}; i < D; ++i)
    {
        m_data[i] = _other[i];
    }
}

template <typename T, Int D>
template <Int D2, typename... Args>
    requires(D2 < D, (sizeof...(Args) == (D - D2) && (std::convertible_to<Args, T> && ...)))
inline Vector<T, D>::Vector(Vector<T, D2> const& _other, Args&&... args) noexcept
{
    Int index{0};
    for (Int i{0}; i < D2; ++i)
    {
        m_data[index++] = _other[i];
    }

    ((m_data[index++] = std::forward<Args>(args)), ...);
}

template <typename T, Int D>
inline Vector<T, D>::Vector(T value) noexcept
{
    for (Int i{0}; i < D; ++i)
    {
        m_data[i] = value;
    }
}

template <typename T, Int D>
inline T& Vector<T, D>::operator[](Int _index) noexcept
{
    return m_data[_index];
}

template <typename T, Int D>
inline T const& Vector<T, D>::operator[](Int _index) const noexcept
{
    return m_data[_index];
}

// 特化访问方法
template <typename T, Int D>
inline T& Vector<T, D>::x() noexcept
    requires(D >= 1)
{
    return (*this)[0];
}

template <typename T, Int D>
inline T const& Vector<T, D>::x() const noexcept
    requires(D >= 1)
{
    return (*this)[0];
}

template <typename T, Int D>
inline T& Vector<T, D>::y() noexcept
    requires(D >= 2)
{
    return (*this)[1];
}

template <typename T, Int D>
inline T const& Vector<T, D>::y() const noexcept
    requires(D >= 2)
{
    return (*this)[1];
}

template <typename T, Int D>
inline T& Vector<T, D>::z() noexcept
    requires(D >= 3)
{
    return (*this)[2];
}

template <typename T, Int D>
inline T const& Vector<T, D>::z() const noexcept
    requires(D >= 3)
{
    return (*this)[2];
}

template <typename T, Int D>
inline T& Vector<T, D>::w() noexcept
    requires(D >= 4)
{
    return (*this)[3];
}

template <typename T, Int D>
inline T const& Vector<T, D>::w() const noexcept
    requires(D >= 4)
{
    return (*this)[3];
}

template <typename T, Int D>
inline T& Vector<T, D>::r() noexcept
    requires(D >= 1)
{
    return (*this)[0];
}

template <typename T, Int D>
inline T const& Vector<T, D>::r() const noexcept
    requires(D >= 1)
{
    return (*this)[0];
}

template <typename T, Int D>
inline T& Vector<T, D>::g() noexcept
    requires(D >= 2)
{
    return (*this)[1];
}

template <typename T, Int D>
inline T const& Vector<T, D>::g() const noexcept
    requires(D >= 2)
{
    return (*this)[1];
}

template <typename T, Int D>
inline T& Vector<T, D>::b() noexcept
    requires(D >= 3)
{
    return (*this)[2];
}
template <typename T, Int D>
inline T const& Vector<T, D>::b() const noexcept
    requires(D >= 3)
{
    return (*this)[2];
}

template <typename T, Int D>
inline T& Vector<T, D>::a() noexcept
    requires(D >= 4)
{
    return (*this)[3];
}

template <typename T, Int D>
inline T const& Vector<T, D>::a() const noexcept
    requires(D >= 4)
{
    return (*this)[3];
}

// 整数类型的比较
template <typename T, Int D>
inline Bool Vector<T, D>::isEqual(Vector<T, D> const& _other) const noexcept
    requires std::integral<T>
{
    for (Int i{0}; i < D; ++i)
    {
        if (m_data[i] != _other[i])
        {
            return false;
        }
    }

    return true;
}

// 浮点类型的比较
template <typename T, Int D>
inline Bool Vector<T, D>::isEqual(Vector<T, D> const& _other, T const _tol) const
    requires std::floating_point<T>
{
    for (Int i{0}; i < D; ++i)
    {
        if (std::fabs(m_data[i] - _other[i]) > _tol)
        {
            return false;
        }
    }
    return true;
}

template <typename T, Int D>
inline T* Vector<T, D>::data() noexcept
{
    return m_data;
}

template <typename T, Int D>
inline T const* Vector<T, D>::data() const noexcept
{
    return m_data;
}

template <typename T, Int D>
inline Vector<T, D> Vector<T, D>::operator*(Vector<T, D> const& _other) const noexcept
{
    Vector<T, D> result{};
    for (Int i{0}; i < D; ++i)
    {
        result[i] = m_data[i] * _other[i];
    }

    return result;
}

template <typename T, Int D>
inline Vector<T, D> Vector<T, D>::operator*(Float const _val) const noexcept
{
    Vector<T, D> result{};
    for (Int i{0}; i < D; ++i)
    {
        result[i] = m_data[i] * _val;
    }

    return result;
}

template <typename T, Int D>
inline Vector<T, D>& Vector<T, D>::operator*=(Float const _val) noexcept
{
    *this = (*this) * _val;
    return *this;
}

template <typename T, Int D>
inline Vector<T, D> Vector<T, D>::operator+(Vector<T, D> const& _other) const noexcept
{
    Vector<T, D> result{};

    for (Int i{0}; i < D; ++i)
    {
        result[i] = m_data[i] + _other[i];
    }

    return result;
}
template <typename T, Int D>
inline Vector<T, D>& Vector<T, D>::operator+=(Vector<T, D> const& _other) noexcept
{
    Vector<T, D>& result{*this};
    result = result + _other;
    return result;
}

template <typename T, Int D>
inline Vector<T, D> Vector<T, D>::operator-(Vector<T, D> const& _other) const noexcept
{
    Vector<T, D> result{};

    for (Int i{0}; i < D; ++i)
    {
        result[i] = m_data[i] - _other[i];
    }

    return result;
}

template <typename T, Int D>
inline Vector<T, D>& Vector<T, D>::operator-=(Vector<T, D> const& _other) noexcept
{
    Vector<T, D>& result{*this};
    result = result - _other;
    return result;
}

template <typename T, Int D>
inline Float Vector<T, D>::dot(Vector<T, D> const& _other) const noexcept
{
    Float result{};

    for (Int i{0}; i < D; ++i)
    {
        result += m_data[i] * _other[i];
    }

    return result;
}

template <typename T, Int D>
inline Vector<T, D>& Vector<T, D>::normalize() noexcept
    requires std::floating_point<T>
{
    T const lengthSquared{this->dot(*this)};

    // TODO: 溢出

    // 防止0向量
    if (lengthSquared > T{0})
    {
        T const inverseLength{1.0f / std::sqrt(lengthSquared)};
        (*this) *= inverseLength;
    }

    return *this;
}

template <typename T, Int D>
inline Vector<T, D> Vector<T, D>::normalized() const noexcept
    requires std::floating_point<T>
{
    Vector<T, D> result{*this};
    return result.normalize();
}

}  // namespace namica