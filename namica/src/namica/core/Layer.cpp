#include "namica/core/Layer.h"

namespace Namica
{
Layer::Layer(char const* _layerName) : m_layerName{_layerName}
{
}

Layer::~Layer()
{
}

void Layer::onAttach()
{
}

void Layer::onDetach()
{
}

void Layer::onUpdate()
{
}

void Layer::onEvent(Event& _event)
{
}

void Layer::onImGuiRender()
{
}

char const* Layer::getName() const noexcept
{
    return m_layerName;
}

}  // namespace Namica