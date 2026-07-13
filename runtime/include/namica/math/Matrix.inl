#pragma once

#include "namica/math/Matrix.h"
#include "namica/math/Vector.h"

namespace namica
{

inline Mat4::Mat4() noexcept
{
}

inline Mat4::Mat4(Float _value) noexcept
    : m_data{{_value, _value, _value, _value},
             {_value, _value, _value, _value},
             {_value, _value, _value, _value},
             {_value, _value, _value, _value}}
{
}

inline Mat4::Mat4(float _v00,
                  float _v01,
                  float _v02,
                  float _v03,
                  float _v10,
                  float _v11,
                  float _v12,
                  float _v13,
                  float _v20,
                  float _v21,
                  float _v22,
                  float _v23,
                  float _v30,
                  float _v31,
                  float _v32,
                  float _v33) noexcept
    : m_data{{_v00, _v01, _v02, _v03},
             {_v10, _v11, _v12, _v13},
             {_v20, _v21, _v22, _v23},
             {_v30, _v31, _v32, _v33}}
{
}

inline Mat4::~Mat4()
{
}

inline float& Mat4::operator()(Int _x, Int _y) noexcept
{
    return m_data[_x][_y];
}

inline float const& Mat4::operator()(Int _x, Int _y) const noexcept
{
    return m_data[_x][_y];
}

inline Float* Mat4::operator[](Int _x) noexcept
{
    return m_data[_x];
}

inline Float const* Mat4::operator[](Int _x) const noexcept
{
    return m_data[_x];
}

// [4, 4] x [4, 1] = [4, 1]
inline Vec4 operator*(Mat4 const& _mat, Vec4 const& _vec)
{
    Vec4 result{};
    result[0] = _mat(0, 0) * _vec[0] + _mat(0, 1) * _vec[1] + _mat(0, 2) * _vec[2] +
        _mat(0, 3) * _vec[3];  // 第一行 x 第一列
    result[1] = _mat(1, 0) * _vec[0] + _mat(1, 1) * _vec[1] + _mat(1, 2) * _vec[2] +
        _mat(1, 3) * _vec[3];  // 第二行 x 第一列
    result[2] = _mat(2, 0) * _vec[0] + _mat(2, 1) * _vec[1] + _mat(2, 2) * _vec[2] +
        _mat(2, 3) * _vec[3];  // 第三行 x 第一列
    result[3] = _mat(3, 0) * _vec[0] + _mat(3, 1) * _vec[1] + _mat(3, 2) * _vec[2] +
        _mat(3, 3) * _vec[3];  // 第四行 x 第一列

    return result;
}

// [1, 4] x [4, 4] = [1, 4]
inline Vec4 operator*(Vec4 const& _vec, Mat4 const& _mat)
{
    Vec4 result{};
    result[0] = _vec[0] * _mat(0, 0) + _vec[1] * _mat(1, 0) + _vec[2] * _mat(2, 0) +
        _vec[3] * _mat(3, 0);  // 第一行 x 第一列
    result[1] = _vec[0] * _mat(0, 1) + _vec[1] * _mat(1, 1) + _vec[2] * _mat(2, 1) +
        _vec[3] * _mat(3, 1);  // 第一行 x 第二列
    result[2] = _vec[0] * _mat(0, 2) + _vec[1] * _mat(1, 2) + _vec[2] * _mat(2, 2) +
        _vec[3] * _mat(3, 2);  // 第一行 x 第三列
    result[3] = _vec[0] * _mat(0, 3) + _vec[1] * _mat(1, 3) + _vec[2] * _mat(2, 3) +
        _vec[3] * _mat(3, 3);  // 第一行 x 第四列

    return result;
}

}  // namespace namica