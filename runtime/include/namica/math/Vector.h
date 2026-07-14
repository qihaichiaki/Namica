#pragma once

#include <type_traits>  // IWYU pragma: keep
#include <cmath>

#include "namica/Common.h"
#include "namica/Base.h"  // IWYU pragma: keep

namespace namica
{

template <typename T, Int D>
class Vector
{
public:
    Vector() noexcept = default;

    template <typename... Args>
        requires(sizeof...(Args) == D && (std::convertible_to<Args, T> && ...))
    explicit Vector(Args&&... args) noexcept : m_data{static_cast<T>((std::forward<Args>(args)))...}
    {
    }

    explicit Vector(T value) noexcept
    {
        for (Int i{0}; i < D; ++i)
        {
            m_data[i] = value;
        }
    }

    T& operator[](Int _index)
    {
        return m_data[_index];
    }

    T const& operator[](Int _index) const
    {
        return m_data[_index];
    }

    // 特化访问方法

    constexpr T& x() noexcept
        requires(D >= 1)
    {
        return (*this)[0];
    }

    constexpr T const& x() const noexcept
        requires(D >= 1)
    {
        return (*this)[0];
    }

    constexpr T& y() noexcept
        requires(D >= 2)
    {
        return (*this)[1];
    }

    constexpr T const& y() const noexcept
        requires(D >= 2)
    {
        return (*this)[1];
    }

    constexpr T& z() noexcept
        requires(D >= 3)
    {
        return (*this)[2];
    }

    constexpr T const& z() const noexcept
        requires(D >= 3)
    {
        return (*this)[2];
    }

    constexpr T& w() noexcept
        requires(D >= 4)
    {
        return (*this)[3];
    }

    constexpr T const& w() const noexcept
        requires(D >= 4)
    {
        return (*this)[3];
    }

    constexpr T& r() noexcept
        requires(D >= 1)
    {
        return (*this)[0];
    }

    constexpr T const& r() const noexcept
        requires(D >= 1)
    {
        return (*this)[0];
    }

    constexpr T& g() noexcept
        requires(D >= 2)
    {
        return (*this)[1];
    }

    constexpr T const& g() const noexcept
        requires(D >= 2)
    {
        return (*this)[1];
    }

    constexpr T& b() noexcept
        requires(D >= 3)
    {
        return (*this)[2];
    }

    constexpr T const& b() const noexcept
        requires(D >= 3)
    {
        return (*this)[2];
    }

    constexpr T& a() noexcept
        requires(D >= 4)
    {
        return (*this)[3];
    }

    constexpr T const& a() const noexcept
        requires(D >= 4)
    {
        return (*this)[3];
    }

    // 整数类型的比较
    [[nodiscard]] Bool isEqual(Vector<T, D> const& _other) const
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
    [[nodiscard]] Bool isEqual(Vector<T, D> const& _other,
                               T const _tol = static_cast<T>(NAMICA_TOL)) const
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

private:
    T m_data[D]{};
};

#ifdef NAMICA_BUILD_DLL

extern template class Vector<Float, 2>;
extern template class Vector<Float, 3>;
extern template class Vector<Float, 4>;

extern template class Vector<Int, 2>;
#else

extern template class NAMICA_API Vector<Float, 2>;
extern template class NAMICA_API Vector<Float, 3>;
extern template class NAMICA_API Vector<Float, 4>;

extern template class NAMICA_API Vector<Int, 2>;
#endif

using Vec2 = Vector<Float, 2>;
using Vec3 = Vector<Float, 3>;
using Vec4 = Vector<Float, 4>;

using Veci2 = Vector<Int, 2>;

}  // namespace namica