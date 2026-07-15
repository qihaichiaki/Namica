#pragma once

#include "namica/math/Vector.h"

namespace namica
{

// 4 x 4矩阵类型

// 列主序矩阵
// R T
// x 1
// vec4 vec4 vec4 vec4
class Mat4
{
public:
    Mat4() noexcept;  // 默认为0矩阵
    explicit Mat4(Float _value) noexcept;
    // _v行列
    Mat4(Float _v00,
         Float _v01,
         Float _v02,
         Float _v03,
         Float _v10,
         Float _v11,
         Float _v12,
         Float _v13,
         Float _v20,
         Float _v21,
         Float _v22,
         Float _v23,
         Float _v30,
         Float _v31,
         Float _v32,
         Float _v33) noexcept;

    Float& operator()(Int _row, Int _column) noexcept;
    Float const& operator()(Int _row, Int _column) const noexcept;

    Vec4& operator[](Int _column) noexcept;
    Vec4 const& operator[](Int _column) const noexcept;

    Float* data() noexcept;
    Float const* data() const noexcept;

private:
    Vec4 m_columns[4]{};  // 4个列向量 columns
};

Vec4 operator*(Mat4 const& _mat, Vec4 const& _vec) noexcept;

/**
 * @brief 对矩阵进行平移转换
 *
 * @param _mat 4x4矩阵
 * @param _offset 位移量
 * @return Mat4 平移转换后的矩阵
 */
Mat4 translate(Mat4 const& _mat, Vec3 const& _offset) noexcept;

/**
 * @brief 对矩阵沿着axis进行旋转变换
 *
 * @param _mat 4x4矩阵
 * @param _angle 旋转角度(弧度值)
 * @param _axis 旋转轴
 * @return Mat4
 */
Mat4 rotate(Mat4 const& _mat, Float const _angle, Vec3 const& _axis) noexcept;

}  // namespace namica

#include "namica/math/Matrix.inl"