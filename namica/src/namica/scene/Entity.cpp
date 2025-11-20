#include "namica/scene/Entity.h"
#include "namica/core/Log.h"

namespace Namica
{
Entity::Entity(entt::entity _entityHandle, Scene* _scene)
    : m_entityHandle{_entityHandle}, m_scene{_scene}
{
    // 创建时, 必须保证实体的有效性
    NAMICA_CORE_ASSERT(m_scene && isValid());
}

bool Entity::isValid() const
{
    return m_scene->m_registry.valid(m_entityHandle);
}

bool Entity::isEquals(Entity const& _other) const
{
    return m_entityHandle == _other.m_entityHandle && m_scene == _other.m_scene;
}
}  // namespace Namica