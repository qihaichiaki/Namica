#include "namica/core/LayerStack.h"

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
    m_layers.emplace(m_layers.begin() + m_layerInsertIndex, _layer);
    m_layerInsertIndex++;
}

void LayerStack::pushOverlay(Layer* _overly)
{
    m_layers.emplace_back(_overly);
}

void LayerStack::popLayer(Layer* _layer)
{
    auto it = std::find(m_layers.begin(), m_layers.end(), _layer);
    if (it != m_layers.end())
    {
        size_t index = it - m_layers.begin();
        if (index < m_layerInsertIndex)
        {
            // 弹出非覆盖层的内容
            m_layerInsertIndex--;
        }
        m_layers.erase(it);
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
