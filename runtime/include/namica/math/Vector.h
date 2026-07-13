#pragma once

#include <type_traits>  // IWYU pragma: keep
#include <cmath>

#include "namica/Common.h"
#include "namica/Base.h"  // IWYU pragma: keep

namespace namica
{

template <Int D>
class Vector
{
public:
    Vector() noexcept = default;

    template <typename... Args>
        requires(sizeof...(Args) == D && (std::convertible_to<Args, Float> && ...))
    explicit Vector(Args&&... args) noexcept
        : m_data{static_cast<Float>((std::forward<Args>(args)))...}
    {
    }

    explicit Vector(Float value) noexcept
    {
        for (Int i{0}; i < D; ++i)
        {
            m_data[i] = value;
        }
    }

    Float& operator[](Int _index)
    {
        return m_data[_index];
    }

    Float const& operator[](Int _index) const
    {
        return m_data[_index];
    }

    Bool isEqual(Vector<D> const& _other, float const _tol = NAMICA_TOL) const
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
    Float m_data[D]{};
};

#ifdef NAMICA_BUILD_DLL

extern template class Vector<2>;
extern template class Vector<3>;
extern template class Vector<4>;
#else

extern template class NAMICA_API Vector<2>;
extern template class NAMICA_API Vector<3>;
extern template class NAMICA_API Vector<4>;
#endif

using Vec2 = Vector<2>;
using Vec3 = Vector<3>;
using Vec4 = Vector<4>;

}  // namespace namica