#pragma once

#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include "namica/core/Memory.h"
#include <string>
#include <entt.hpp>

namespace Namica
{

class Entity;

class Scene
{
public:
    NAMICA_API ~Scene();

    NAMICA_API Entity createEntity(std::string const& _name = "新实体");

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