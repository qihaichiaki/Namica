#pragma once

#include "namica/core/Base.h"
#include "namica/core/Log.h"
#include "namica/core/UUID.h"
#include "namica/scene/Scene.h"
#include "namica/scene/Components.h"
#include <entt.hpp>

namespace Namica
{

class EntityIterator;

class Entity
{
public:
    NAMICA_API Entity();
    NAMICA_API Entity(entt::entity _entityHandle, Scene* _scene);

    /**
     * @brief 实体对象是否有效
     *
     * @return 实体是否存在
     * @note 每次使用实体前都应该进行检查一下, 因为存在空实体或者其场景将其释放掉了
     */
    NAMICA_API bool isValid() const;

    /// @brief 判断两个实体是否一致
    NAMICA_API bool isEquals(Entity const& _other) const;

    /**
     * @brief 查询当前entity是否存在对应的组件
     *
     * @tparam T 组件类型
     * @return 是否存在
     */
    template <typename... T>
    bool hasComponent()
    {
        return m_scene->m_registry.all_of<T...>(m_entityHandle);
    }

    /**
     * @brief 给Entity添加组件数据
     *
     * @tparam T 组件类型
     * @tparam Args 构造组件的参数类型包
     * @param _args 构造组件的参数包
     * @return T& 返回构造处的组件对象
     * @note 注意如果重复添加同种类型的组件, 则会覆盖上一次的组件数据
     */
    template <typename T, typename... Args>
    T& addComponent(Args&&... _args)
    {
        return m_scene->m_registry.emplace_or_replace<T>(m_entityHandle,
                                                         std::forward<Args>(_args)...);
    }

    /**
     * @brief 获取对应类型的组件数据
     *
     * @tparam T 组件类型
     * @return T& 组件对象
     */
    template <typename T>
    T& getComponent()
    {
        // 组件类型必须被当前实体所添加
        NAMICA_CORE_ASSERT(hasComponent<T>());
        return m_scene->m_registry.get<T>(m_entityHandle);
    }

    /**
     * @brief 删除对应类型的组件数据
     *
     * @tparam T 组件类型
     * @note 不允许删除ID和Tag组件类型
     */
    template <typename T>
        requires(!std::same_as<T, IDComponent> && !std::same_as<T, TagComponent>)
    void removeComponent()
    {
        // 组件类型必须被当前实体所添加
        NAMICA_CORE_ASSERT(hasComponent<T>());
        m_scene->m_registry.remove<T>(m_entityHandle);
    }

    /**
     * @brief 返回当前实体存在的场景对象
     *
     * @return 场景引用
     */
    NAMICA_API Scene const& getScene() const;

    /**
     * @brief 返回当前实体的UUID
     */
    NAMICA_API UUID getUUID();

    /**
     * @brief 获取当前实体的名字
     */
    NAMICA_API const std::string& getName();

    /**
     * @brief 获取当前的实体的父实体
     */
    NAMICA_API Entity getParent();

    /**
     * @brief 获取当前实体的子实体们
     *
     * @note 每次获取都要创建新的entity arr, 慎用
     */
    NAMICA_API std::vector<Entity> getChildren();

    /**
     * @brief 获取当前实体子实体们的迭代器
     */
    NAMICA_API EntityIterator getChildrenIterator();

    NAMICA_API
    operator entt::entity() const;
    NAMICA_API operator uint32_t() const;
    NAMICA_API operator bool() const;
    NAMICA_API bool operator==(Entity const& _other) const;
    NAMICA_API bool operator!=(Entity const& _other) const;

private:
    entt::entity m_entityHandle{entt::null};
    Scene* m_scene{nullptr};
};

}  // namespace Namica