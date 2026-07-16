#pragma once

#include "namica/math/Matrix.h"

namespace namica
{

inline Mat4::Mat4() noexcept
{
}

inline Mat4::Mat4(Float _value) noexcept
{
    // _value 0.0f   0.0f   0.0f
    // 0.0f   _value 0.0f   0.0f
    // 0.0f   0.0f   _value 0.0f
    // 0.0f   0.0f   0.0f   _value

    m_columns[0][0] = _value;
    m_columns[1][1] = _value;
    m_columns[2][2] = _value;
    m_columns[3][3] = _value;
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
    : m_columns{Vec4{_v00, _v10, _v20, _v30},
                Vec4{_v01, _v11, _v21, _v31},
                Vec4{_v02, _v12, _v22, _v32},
                Vec4{_v03, _v13, _v23, _v33}}
{
}

inline Float& Mat4::operator()(Int _row, Int _column) noexcept
{
    return m_columns[_column][_row];
}

inline Float const& Mat4::operator()(Int _row, Int _column) const noexcept
{
    return m_columns[_column][_row];
}

inline Vec4& Mat4::operator[](Int _column) noexcept
{
    return m_columns[_column];
}

inline Vec4 const& Mat4::operator[](Int _column) const noexcept
{
    return m_columns[_column];
}

inline Float* Mat4::data() noexcept
{
    return m_columns[0].data();
}

inline Float const* Mat4::data() const noexcept
{
    return m_columns[0].data();
}

// [4, 4] x [4, 1] = [4, 1]
inline Vec4 Mat4::operator*(Vec4 const& _vec) const noexcept
{
    Mat4 const& mat{*this};
    Vec4 result{};
    result[0] = mat(0, 0) * _vec[0] + mat(0, 1) * _vec[1] + mat(0, 2) * _vec[2] +
        mat(0, 3) * _vec[3];  // 第一行 x 第一列
    result[1] = mat(1, 0) * _vec[0] + mat(1, 1) * _vec[1] + mat(1, 2) * _vec[2] +
        mat(1, 3) * _vec[3];  // 第二行 x 第一列
    result[2] = mat(2, 0) * _vec[0] + mat(2, 1) * _vec[1] + mat(2, 2) * _vec[2] +
        mat(2, 3) * _vec[3];  // 第三行 x 第一列
    result[3] = mat(3, 0) * _vec[0] + mat(3, 1) * _vec[1] + mat(3, 2) * _vec[2] +
        mat(3, 3) * _vec[3];  // 第四行 x 第一列

    return result;
}

// 只保留列主序, 后续存在需求进行增加
// [1, 4] x [4, 4] = [1, 4]
// inline Vec4 operator*(Vec4 const& _vec, Mat4 const& _mat)
// {
//     Vec4 result{};
//     result[0] = _vec[0] * _mat(0, 0) + _vec[1] * _mat(1, 0) + _vec[2] * _mat(2, 0) +
//         _vec[3] * _mat(3, 0);  // 第一行 x 第一列
//     result[1] = _vec[0] * _mat(0, 1) + _vec[1] * _mat(1, 1) + _vec[2] * _mat(2, 1) +
//         _vec[3] * _mat(3, 1);  // 第一行 x 第二列
//     result[2] = _vec[0] * _mat(0, 2) + _vec[1] * _mat(1, 2) + _vec[2] * _mat(2, 2) +
//         _vec[3] * _mat(3, 2);  // 第一行 x 第三列
//     result[3] = _vec[0] * _mat(0, 3) + _vec[1] * _mat(1, 3) + _vec[2] * _mat(2, 3) +
//         _vec[3] * _mat(3, 3);  // 第一行 x 第四列

//     return result;
// }

inline Mat4 Mat4::operator*(Mat4 const& _other) const noexcept
{
    Mat4 const& mat{*this};
    Mat4 result{};

    result[0] = mat * _other[0];
    result[1] = mat * _other[1];
    result[2] = mat * _other[2];
    result[3] = mat * _other[3];

    return result;
}

inline Mat4& Mat4::operator*=(Mat4 const& _other) noexcept
{
    *this = (*this) * _other;
    return *this;
}

inline Mat4& Mat4::translate(Vec3 const& _offset) noexcept
{
    Mat4& result{*this};

    // R T'  x  1 T
    // x 1      0 1
    //
    // 由于: result的第j列 = m x T的第j列 (矩阵运算, 左行 x 右列)
    // (注意: 注意下面的()表示的列向量, []表示第几列)
    // result[0] = m x (1, 0, 0, 0) => m[0]
    // result[1] = m x (0, 1, 0, 0) => m[1]
    // result[2] = m x (0, 0, 1, 0) => m[2]
    // result[3] = m x (Tx, Ty, Tz, 1.0f)
    // 其中 矩阵m4x4 和 列向量T4x1可以分解为: m[0] * Tx + m[1] * Ty + m[2] * Tz + m[3] * 1.0f
    // 比如 2x2 的和 2x1相乘:
    // a b  x  e  = ae + bf => (a, c) * e + (b, d) * f
    // c d     f    ce + df

    result[3] =
        result[0] * _offset.x() + result[1] * _offset.y() + result[2] * _offset.z() + result[3];
    return result;
}

inline Mat4 Mat4::translated(Vec3 const& _offset) const noexcept
{
    Mat4 result{*this};
    return result.translate(_offset);
}

inline Mat4& Mat4::rotate(Float const _angle, Vec3 const& _axis) noexcept
{
    // 如果是0向量直接返回
    if (_axis.isEqual(Vec3{0.0f}))
    {
        return *this;
    }

    // 使用Rodrigues公式计算
    Vec3 const axis{_axis.normalized()};

    Float const x{axis.x()};
    Float const y{axis.y()};
    Float const z{axis.z()};

    Float const c{std::cos(_angle)};
    Float const s{std::sin(_angle)};
    Float const t{1.0f - c};

    Mat4 rotation{1.0f};

    // column, row
    rotation[0][0] = c + t * x * x;
    rotation[0][1] = t * x * y + s * z;
    rotation[0][2] = t * x * z - s * y;

    rotation[1][0] = t * x * y - s * z;
    rotation[1][1] = c + t * y * y;
    rotation[1][2] = t * y * z + s * x;

    rotation[2][0] = t * x * z + s * y;
    rotation[2][1] = t * y * z - s * x;
    rotation[2][2] = c + t * z * z;

    return (*this) *= rotation;
}

inline Mat4 Mat4::rotated(Float const _angle, Vec3 const& _axis) const noexcept
{
    Mat4 mat{*this};
    return mat.rotate(_angle, _axis);
}

inline Mat4& Mat4::scale(Vec3 const& _scale) noexcept
{
    Mat4& result{*this};

    result[0] = result[0] * _scale.x();
    result[1] = result[1] * _scale.y();
    result[2] = result[2] * _scale.z();
    // result[3] = result[3] * 1.0f;

    return result;
}

inline Mat4 Mat4::scaled(Vec3 const& _scale) const noexcept
{
    Mat4 result{*this};
    return result.scale(_scale);
}

}  // namespace namica