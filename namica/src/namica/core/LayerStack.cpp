#include "namica/core/LayerStack.h"
#include "namica/core/Log.h"

namespace Namica
{
LayerStack::LayerStack()
{
}

LayerStack::~LayerStack()
{
}

void LayerStack::pushLayer(Layer* _layer)
{
    NAMICA_CORE_ASSERT(_layer);
    _layer->onAttach();

    m_layers.emplace(m_layers.begin() + m_layerInsertIndex, _layer);
    m_layerInsertIndex++;
}

void LayerStack::pushOverlay(Layer* _overly)
{
    NAMICA_CORE_ASSERT(_overly);
    _overly->onAttach();

    m_layers.emplace_back(_overly);
}

void LayerStack::popLayer(Layer* _layer)
{
    NAMICA_CORE_ASSERT(_layer);

    auto it = std::find(m_layers.begin(), m_layers.end(), _layer);
    if (it != m_layers.end())
    {
        m_layers.erase(it);
        m_layerInsertIndex--;
        _layer->onDetach();
    }
}

void LayerStack::popOverlay(Layer* _overly)
{
    NAMICA_CORE_ASSERT(_overly);

    auto it = std::find(m_layers.begin(), m_layers.end(), _overly);
    if (it != m_layers.end())
    {
        m_layers.erase(it);
        _overly->onDetach();
    }
}

std::vector<Layer*>::iterator LayerStack::begin()
{
    return m_layers.begin();
}

std::vector<Layer*>::iterator LayerStack::end()
{
    return m_layers.end();
}
}  // namespace Namica
