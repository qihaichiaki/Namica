#pragma once

#include "namica_editor/EditorContext.h"
#include "namica_editor/EditorMainUI.h"
#include <namica/core/Layer.h>
#include <namica/renderer/Texture.h>
#include <namica/events/WindowEvent.h>

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
    virtual void onEvent(Event& _event) override;
    virtual void onImGuiRender() override;

private:
    EditorContext m_context;
    EditorMainUI m_mainUI;
    Ref<Texture2D> m_testTetxure;
};

}  // namespace Namica