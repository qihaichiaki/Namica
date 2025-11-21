#pragma once

#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include "namica/core/Memory.h"
#include "namica/core/UUID.h"
#include <string>
#include <entt.hpp>

namespace Namica
{

class Entity;

class Scene
{
public:
    NAMICA_API ~Scene();

    /**
     * @brief 从当前场景创建一个实体对象
     *
     * @param _name 实体对象tag组件中的名字
     * @return 对象标识
     * @note 注意创建出的对象默认存在tag, transform组件
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

public:
    NAMICA_API static Ref<Scene> create();

private:
    Scene();

private:
    entt::registry m_registry;  // entt注册管理器, 管理组件和实体

    friend class Entity;
    // 禁止移动和拷贝对象
    CLASS_DISABLE_COPY_MOVE(Scene)
    // createRef成为Scne的友元函数
    FRIEND_REF_FUNC
};

}  // namespace Namica