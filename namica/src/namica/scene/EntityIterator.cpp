#include "namica/scene/EntityIterator.h"
#include "namica/scene/Entity.h"

namespace Namica
{
EntityIterator::EntityIterator(std::vector<entt::entity> const* _enids, Scene* _scene)
    : m_enids{_enids}, m_scene{_scene}
{
}

Entity EntityIterator::current()
{
    return Entity{(*m_enids)[index], m_scene};
}

void EntityIterator::next()
{
    ++index;
}

bool EntityIterator::hasNext()
{
    return m_enids && index < m_enids->size();
}
}  // namespace Namica