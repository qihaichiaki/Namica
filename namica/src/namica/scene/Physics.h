#pragma once

#include "namica/core/Base.h"  // IWYU pragma: keep: use int32_t...

namespace Namica
{

// box2d世界中的标识符
struct B2BodyHandle
{
    int32_t index1{0};
    uint16_t world0{0};
    uint16_t generation{0};
};

// 物理材质
struct PhysicalMaterials
{
    float density{1.0f};      // 密度
    float friction{0.5f};     // 摩擦力
    float restitution{0.0f};  // 反弹系数
    // TODO: 不知道是否需要
    float restitutionThreshold{0.5f};  // 反弹恢复阈值
};

};  // namespace Namica