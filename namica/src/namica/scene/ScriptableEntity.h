#pragma once

#include "namica/core/Time.h"
#include "namica/scene/Entity.h"

namespace Namica
{

class ScriptableEntity
{
public:
    /**
     * @brief 返回当前附着的实体对象的相应类型组件
     *
     * @tparam T 组件类型
     * @return T& 组件对象
     */
    template <typename T>
    T& getComponent()
    {
        return m_entity.getComponent<T>();
    }

public:
    virtual void onCreate()
    {
    }

    virtual void onDestroy()
    {
    }

    virtual void onUpdate(Timestep)
    {
    }

private:
    Entity m_entity;

    friend class Scene;
};

}  // namespace Namica