#pragma once

#include <namica/core/Types.h>
#include <namica/math/Matrix.h>

#include <gtest/gtest.h>

// 数学测试中的默认绝对误差；累计变换等用例可显式传入更合适的容差。
constexpr namica::Float kEpsilon{1e-6f};

// 按分量比较同维浮点向量，失败时报告分量索引，避免单个布尔结果掩盖误差位置。
template <namica::Int D>
void ExpectVectorNear(namica::Vector<namica::Float, D> const& _actual,
                      namica::Vector<namica::Float, D> const& _expected,
                      namica::Float const _epsilon = kEpsilon)
{
    for (namica::Int index{0}; index < D; ++index)
    {
        EXPECT_NEAR(_actual[index], _expected[index], _epsilon) << "component = " << index;
    }
}

// 按行列比较矩阵的全部元素；只做数值断言，不修改输入或判断矩阵用途。
void ExpectMat4Near(namica::Mat4 const& _actual,
                    namica::Mat4 const& _expected,
                    namica::Float const _epsilon = kEpsilon);
