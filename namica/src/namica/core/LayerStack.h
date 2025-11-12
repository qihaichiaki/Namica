#pragma once

#include "namica/core/Base.h"
#include "namica/core/Layer.h"
#include <vector>

namespace Namica
{

class LayerStack
{
public:
    NAMICA_API LayerStack();
    NAMICA_API ~LayerStack();

    NAMICA_API void pushLayer(Layer* _layer);
    NAMICA_API void pushOverlay(Layer* _overly);
    NAMICA_API void popLayer(Layer* _layer);
    NAMICA_API std::vector<Layer*>::iterator begin();
    NAMICA_API std::vector<Layer*>::iterator end();

private:
    std::vector<Layer*> m_layers;
    unsigned int m_layerInsertIndex{0};
};
}  // namespace Namica