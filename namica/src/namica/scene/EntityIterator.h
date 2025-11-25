#pragma once

#include "namica/core/Base.h"

#include <vector>
#include <entt.hpp>

namespace Namica
{

class Entity;
class Scene;

class EntityIterator
{
public:
    EntityIterator() = default;
    EntityIterator(std::vector<entt::entity>*, Scene*);

    NAMICA_API Entity current();
    NAMICA_API void next();
    NAMICA_API bool hasNext();

private:
    std::vector<entt::entity>* m_enids{nullptr};
    Scene* m_scene{nullptr};
    uint32_t index{0};
};

}  // namespace Namica