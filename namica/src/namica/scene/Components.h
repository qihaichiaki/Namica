#pragma once

#include "namica/core/UUID.h"
#include "namica/renderer/Texture.h"
#include "namica/renderer/Camera.h"
#include "namica/scene/Physics.h"

#include <string>
#include <vector>
#include <entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Namica
{

/**
 * @brief 唯一实体标识组件
 *
 */
struct IDComponent
{
    UUID id{};
};

/**
 * @brief 实体标识组件, 包含ID和实体名字
 */
struct TagComponent
{
    std::string name{};
};

/**
 * @brief 实体变换组件, 表示其在世界中的具体变换
 */
struct TransformComponent
{
    glm::vec3 translation{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    glm::mat4 getTransform() const
    {
        auto rotationMat4 = glm::toMat4(glm::quat(rotation));  // 使用欧拉角
        return glm::translate(glm::mat4{1.0f}, translation) * rotationMat4 *
            glm::scale(glm::mat4{1.0f}, scale);
    }

    void setTransform(glm::mat4 const& transform)
    {
        glm::quat orientation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transform, scale, orientation, translation, skew, perspective);
        rotation = glm::eulerAngles(orientation);
    }
};

/**
 * @brief 实体中的关系组件, 用来管理entity之间层级关系
 */
struct RelationshipComponent
{
    entt::entity parent{entt::null};       // 父entity的标识
    std::vector<entt::entity> children{};  // 子entity们
};

struct CameraComponent
{
    Camera camera{};

    // 是否是主相机, 运行时第一个主相机起到运行时的渲染作用
    bool primary{true};
    // 是否固定渲染纵横比(true的话运行时候不会更新其camera中的渲染视图宽度和高度)
    bool fixedAspectRatio{false};
};

/**
 * @brief 精灵图组件, 用来显示png/jpg图片
 */
struct SpriteRendererComponent
{
    glm::vec4 color{1.0f};
    Ref<Texture2D> texture{nullptr};
    float tilingFactor{1.0f};
};

/**
 * @brief 圆形渲染组件, 用来显示圆
 */
struct CircleRendererComponent
{
    glm::vec4 color{1.0f};
    float thickness{1.0f};  // [0.0f, 1.0f] 表示厚度, 1为实心圆
    float fade{0.005f};     // 表示圆的模糊效果
};

// 脚本实体前置声明
class ScriptableEntity;

/**
 * @brief 原生脚本组件, 使用C++的脚本
 */
struct NativeScriptComponent
{
    ScriptableEntity* instance{nullptr};  // 脚本示例, 在引擎运行时会被创建

    ScriptableEntity* (*instantiateScript)() = nullptr;  // 延迟创建脚本对象函数
    void (*destroyScript)(ScriptableEntity*) = nullptr;  // 销毁脚本对象函数

    /**
     * @brief 脚本对象绑定实际脚本类
     *
     * @tparam T 自定义脚本类, 需要继承自ScriptableEntity
     */
    template <typename T>
        requires std::is_base_of_v<ScriptableEntity, T>
    void bind()
    {
        instantiateScript = []() -> ScriptableEntity* { return new T{}; };
        destroyScript = [](ScriptableEntity* _instance) -> void {
            delete static_cast<T*>(_instance);
        };
    }
};

/**
 * @brief 2D刚体组件, 用于控制对象的2D刚体物理世界生命周期
 */
struct Rigidbody2DComponent
{
    Physics2D::RigidbodyType type{Physics2D::RigidbodyType::Static};
    bool fixedRotation{false};  // 是否锁定对象在z轴上的旋转

    Physics2D::BodyHandle bodyId{};
};

/**
 * @brief 2D方形碰撞箱
 */
struct BoxCollider2DComponent
{
    glm::vec2 offset{0.0f};
    glm::vec2 size{0.0f};
    float rotation{0.0f};

    // 物理材质
    Physics2D::Materials physicalMaterials{};
};

/**
 * @brief 2D圆形碰撞箱
 */
struct CircleCollider2DComponent
{
    glm::vec2 offset{0.0f, 0.0f};
    float radius{0.0f};

    Physics2D::Materials physicalMaterials{};
};

}  // namespace Namica