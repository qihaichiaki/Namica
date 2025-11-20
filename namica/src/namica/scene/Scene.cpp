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

}  // namespace Namica