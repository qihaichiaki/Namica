#pragma once

#include "namica/core/UUID.h"
#include "namica/renderer/Texture.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Namica
{

/**
 * @brief 实体标识组件, 包含ID和实体名字
 */
struct TagComponent
{
    UUID id{};
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

    void (*instantiateScript)(NativeScriptComponent*) = nullptr;  // 延迟创建脚本对象函数
    void (*destroyScript)(NativeScriptComponent*) = nullptr;      // 销毁脚本对象函数

    /**
     * @brief 脚本对象绑定实际脚本类
     *
     * @tparam T 自定义脚本类, 需要继承自ScriptableEntity
     */
    template <typename T>
        requires std::is_base_of_v<ScriptableEntity, T>
    void bind()
    {
        instantiateScript = [](NativeScriptComponent* _other) -> void {
            _other->instance = new T{};
        };
        destroyScript = [](NativeScriptComponent* _other) -> void {
            delete static_cast<T*>(_other->instance);
            _other->instance = nullptr;
        };
    }
};

}  // namespace Namica