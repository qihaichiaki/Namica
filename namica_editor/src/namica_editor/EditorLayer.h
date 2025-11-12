#pragma once

#include "namica/core/Layer.h"

namespace Namica
{

class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate() override;
};

}  // namespace Namica