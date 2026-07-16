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

    [[nodiscard]] Float& operator()(Int _row, Int _column) noexcept;
    [[nodiscard]] Float const& operator()(Int _row, Int _column) const noexcept;

    [[nodiscard]] Vec4& operator[](Int _column) noexcept;
    [[nodiscard]] Vec4 const& operator[](Int _column) const noexcept;

    [[nodiscard]] Float* data() noexcept;
    [[nodiscard]] Float const* data() const noexcept;

    // 矩阵相关运算
    [[nodiscard]] Vec4 operator*(Vec4 const& _vec) const noexcept;
    [[nodiscard]] Mat4 operator*(Mat4 const& _other) const noexcept;
    Mat4& operator*=(Mat4 const& _other) noexcept;

    /**
     * @brief 对当前矩阵进行平移转换
     *
     * @param _offset 位移量
     * @return 平移变换后的矩阵
     */
    Mat4& translate(Vec3 const& _offset) noexcept;
    [[nodiscard]] Mat4 translated(Vec3 const& _offset) const noexcept;

    /**
     * @brief 对当前矩阵沿着axis进行旋转变换
     *
     * @param _angle 旋转角度(弧度值)
     * @param _axis 旋转轴(在运算时会归一化)
     * @return 旋转变换后的矩阵
     */
    Mat4& rotate(Float const _angle, Vec3 const& _axis) noexcept;
    [[nodiscard]] Mat4 rotated(Float const _angle, Vec3 const& _axis) const noexcept;

    /**
     * @brief 对当前矩阵进行缩放变换
     *
     * @param _scale 缩放值
     * @return 缩放变换后的矩阵
     */
    Mat4& scale(Vec3 const& _scale) noexcept;
    [[nodiscard]] Mat4 scaled(Vec3 const& _scale) const noexcept;

private:
    Vec4 m_columns[4]{};  // 4个列向量 columns
};

}  // namespace namica

#include "namica/math/Matrix.inl"