#pragma once

#include "namica/core/Base.h"
#include "namica/core/Time.h"
#include <glm/glm.hpp>

namespace Namica
{

namespace Physics2D
{

// box2d世界中的标识符
struct WorldHandle
{
    uint16_t index1{0};
    uint16_t generation{0};
};

enum class RigidbodyType
{
    Static,    // 完全静止
    Dynamic,   // 运动的
    Kinematic  // 不受力的作用
};

struct BodyHandle
{
    int32_t index1{0};
    uint16_t world0{0};
    uint16_t generation{0};
};

// 物理材质
struct Materials
{
    float density{1.0f};      // 密度
    float friction{0.5f};     // 摩擦力
    float restitution{0.0f};  // 反弹系数
    // TODO: 不知道是否需要
    float restitutionThreshold{0.5f};  // 反弹恢复阈值
};

// 物理世界运作中心, box2d的简单封装
class PhysicsWorld
{
public:
    /// @brief 物理世界初始化
    NAMICA_API void init();

    /// @brief 物理世界更新
    NAMICA_API void onUpdate(Timestep _ts);

    /// @brief 物理世界销毁
    NAMICA_API void shutdown();

    /// @brief 物理世界是否运行
    NAMICA_API bool isRunning() const;

    /**
     * @brief 创建物理世界中对象的初始状态
     *
     * @param _type 对象类型
     * @param _position 对象初始位置, x, y
     * @param _rotation 对象旋转弧度值
     * @param _fixedRotation 对象是否冻结旋转
     *
     * @return 物理世界中对象句柄
     */
    NAMICA_API BodyHandle createBody(RigidbodyType _type,
                                     glm::vec2 const& _position,
                                     float _rotation,
                                     bool _fixedRotation);

    /**
     * @brief 将对象从物理世界中进行移除
     *
     * @param _body 待移除的对象
     * @return 删除是否成功
     */
    NAMICA_API bool destroyBody(BodyHandle const& _body);

    /**
     * @brief 为物理世界中的对象添加box形状
     *
     * @param _body 物理世界中对象句柄
     * @param _offset box形状的偏移
     * @param _halfSize box形状的半边大小
     * @param _rotation box形状的旋转
     * @param _materials box形状的材质
     */
    void attachBodyBoxShape(BodyHandle const& _body,
                            glm::vec2 const& _offset,
                            glm::vec2 const& _halfSize,
                            float _rotation,
                            Materials const& _materials);

    /**
     * @brief 为物理世界中的对象添加circle形状
     *
     * @param _body 物理世界中对象句柄
     * @param _offset circle形状的偏移
     * @param _radius circle形状的半径
     * @param _materials circle形状的材质
     */
    void attacheBodyCircleShape(BodyHandle const& _body,
                                glm::vec2 const& _offset,
                                float _radius,
                                Materials const& _materials);

    // TODO: 运行时随时修改方法

    /**
     * @brief 获取物理更新后, 物理对象的transform
     *
     * @param[in] _body 物理世界中对象句柄
     * @param[out] _position 位置(z方向不变)
     * @param[out] _rotation 旋转
     */
    void getBodyTransform(BodyHandle const& _body, glm::vec3& _position, float& _rotation);

private:
    WorldHandle m_worldHandle;
    bool m_isRunning{false};
};

}  // namespace Physics2D

};  // namespace Namica