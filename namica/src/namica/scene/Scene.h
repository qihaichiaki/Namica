#pragma once

#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include "namica/core/Memory.h"
#include "namica/core/UUID.h"
#include "namica/core/Time.h"
#include "namica/scene/Physics.h"
#include <string>
#include <entt.hpp>

namespace Namica
{

class Entity;
class EditorCamera;

class Scene
{
public:
    NAMICA_API ~Scene();

    /**
     * @brief 判断传入的entity是在当前场景存在
     *
     * @param _entity 实体对象
     * @return 是否在当前场景存在
     */
    NAMICA_API bool containsEntity(Entity _entity) const;

    /**
     * @brief 从当前场景创建一个实体对象
     *
     * @param _name 实体对象tag组件中的名字
     * @return 对象标识
     * @note 注意创建出的对象默认存在id, tag, transform组件
     */
    NAMICA_API Entity createEntity(std::string const& _name = "新实体");
    /**
     * @brief 根据传入的UUID创建一个实体对象
     *
     * @param _uuid 全局唯一标识符
     * @param _name 实体对象在tag组件中的名字
     * @return 对象标识
     * @note 注意创建出的对象默认存在tag, transform组件
     */
    NAMICA_API Entity createEntity(const UUID& _uuid, std::string const& _name = "新实体");

    /**
     * @brief 从当前场景删除对应的实体对象
     *
     * @param _entity 待删除实体对象
     * @return 删除实体是否成功
     */
    NAMICA_API bool destoryEntity(Entity _entity);

    /**
     * @brief 从当前场景复制一个实体对象
     *
     * @param _entity 被复制实体对象
     * @return 全新的实体对象(组件数据和被复制实体对象保持一致)
     */
    NAMICA_API Entity copyEntity(Entity _entity);

    /**
     * @brief 更新当前场景的渲染视口宽度和高度(主要是控制场景内存在camera组件的更新事件)
     *
     * @param _width 宽度
     * @param _height 高度
     */
    NAMICA_API void onViewportResize(uint32_t _width, uint32_t _height);

    /**
     * @brief 场景的编辑器状态更新
     *
     * @param _ts 帧间隔
     * @param _editorCamera 编辑器相机
     */
    NAMICA_API void onUpdateEditor(Timestep _ts, EditorCamera const& _editorCamera);

    /**
     * @brief 开始场景运行时
     */
    NAMICA_API void onStartRuntime();

    /**
     * @brief 场景的运行时状态更新
     *
     * @param _ts 帧间隔
     */
    NAMICA_API void onUpdateRuntime(Timestep _ts);

    /**
     * @brief 终止场景运行时
     */
    NAMICA_API void onStopRuntime();

    /**
     * @brief 设置是否绘制碰撞箱线框
     */
    NAMICA_API void setDrawColliders2D(bool _enable);

public:
    /**
     * @brief 创建一个场景对象
     */
    NAMICA_API static Ref<Scene> create();

    /**
     * @brief 通过一个场景, 拷贝出一个新的场景
     *
     * @param _other 原型场景
     * @return 拷贝后的新场景
     */
    NAMICA_API static Ref<Scene> copy(Ref<Scene> const& _other);

private:
    void onCameraComponentAdded(entt::registry& _registry, entt::entity _enid);
    void onRigidbody2DComponentAdded(entt::registry& _registry, entt::entity _enid);
    void onBoxCollider2DComponentAdded(entt::registry& _registry, entt::entity _enid);
    void onCircleCollider2DComponentAdded(entt::registry& _registry, entt::entity _enid);

    void onRenderer(glm::mat4 const& _projectionView);

    Scene();

private:
    entt::registry m_registry;               // entt注册管理器, 管理组件和实体
    Physics2D::PhysicsWorld m_physicsWorld;  // 2D物理世界

    bool m_isDrawColliders2D{false};  // 是否绘制2d碰撞器线框
    uint32_t m_viewportWidth{0};
    uint32_t m_viewportHeight{0};

    friend class Entity;
    // 禁止移动和拷贝对象
    CLASS_DISABLE_COPY_MOVE(Scene)
    // createRef成为Scne的友元函数
    FRIEND_REF_FUNC
};

}  // namespace Namica