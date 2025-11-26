#include "namica/scene/Entity.h"
// #include "namica/core/Log.h"
#include "namica/scene/EntityIterator.h"

namespace Namica
{

Entity::Entity()
{
}

Entity::Entity(entt::entity _entityHandle, Scene* _scene)
    : m_entityHandle{_entityHandle}, m_scene{_scene}
{
    // 创建时, 必须保证实体的有效性
    // NAMICA_CORE_ASSERT(m_scene && isValid());
}

bool Entity::isValid() const
{
    return m_scene && m_scene->m_registry.valid(m_entityHandle);
}

bool Entity::isEquals(Entity const& _other) const
{
    return m_entityHandle == _other.m_entityHandle && m_scene == _other.m_scene;
}

Scene const& Entity::getScene() const
{
    return *m_scene;
}

UUID Entity::getUUID()
{
    return getComponent<IDComponent>().id;
}

std::string& Entity::getName()
{
    return getComponent<TagComponent>().name;
}

Entity Entity::getParent()
{
    Entity parent{};
    if (hasComponent<RelationshipComponent>())
    {
        parent = Entity{getComponent<RelationshipComponent>().parent, m_scene};
    }

    return parent;
}

std::vector<Entity> Entity::getChildren()
{
    std::vector<Entity> children{};
    if (hasComponent<RelationshipComponent>())
    {
        RelationshipComponent& relationship{getComponent<RelationshipComponent>()};
        size_t const childrenSize{relationship.children.size()};
        children.resize(childrenSize);
        for (size_t i{0}; i < childrenSize; ++i)
        {
            children[i] = Entity{relationship.children[i], m_scene};
        }
    }
    return children;
}

EntityIterator Entity::getChildrenIterator()
{
    if (hasComponent<RelationshipComponent>())
    {
        RelationshipComponent& relationship{getComponent<RelationshipComponent>()};
        return EntityIterator{&(relationship.children), m_scene};
    }

    return EntityIterator{};
}

Entity::operator entt::entity() const
{
    return m_entityHandle;
}

Entity::operator uint32_t() const
{
    return static_cast<uint32_t>(m_entityHandle);
}

Entity::operator bool() const
{
    return isValid();
}

bool Entity::operator==(Entity const& _other) const
{
    return isEquals(_other);
}

bool Entity::operator!=(Entity const& _other) const
{
    return !isEquals(_other);
}

}  // namespace Namica