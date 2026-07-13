#pragma once

#include "namica/Base.h"
#include "namica/Common.h"

namespace namica
{

// 4 x 4矩阵类型
class NAMICA_API Mat4
{
public:
    Mat4() noexcept;  // 默认为单位矩阵
    Mat4(Float _value) noexcept;
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

    ~Mat4();

    Float& operator()(Int _x, Int _y) noexcept;
    Float const& operator()(Int _x, Int _y) const noexcept;
    Float* operator[](Int _x) noexcept;
    Float const* operator[](Int _x) const noexcept;

private:
    Float m_data[4][4]{{1.0f, 0.0f, 0.0f, 0.0f},
                       {0.0f, 1.0f, 0.0f, 0.0f},
                       {0.0f, 0.0f, 1.0f, 0.0f},
                       {0.0f, 0.0f, 0.0f, 1.0f}};
};

}  // namespace namica

#include "namica/math/Matrix.inl"