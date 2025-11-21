#include "namica/scene/Scene.h"
#include "namica/scene/Entity.h"

namespace Namica
{

Scene::Scene()
{
}

Scene::~Scene()
{
}

Ref<Scene> create()
{
    return createRef<Scene>();
}

Entity Scene::createEntity(std::string const& _name)
{
    Entity entity{m_registry.create(), this};
    return entity;
}

Entity Scene::createEntity(const UUID& _uuid, std::string const& _name)
{
    Entity entity{m_registry.create(), this};
    return entity;
}

bool Scene::destoryEntity(Entity _entity)
{
    if (m_registry.valid(_entity))
    {
        m_registry.destroy(_entity);
        return true;
    }

    return false;
}

Entity Scene::copyEntity(Entity _entity)
{
    Entity newEntity{m_registry.create(), this};
    return newEntity;
}

}  // namespace Namica