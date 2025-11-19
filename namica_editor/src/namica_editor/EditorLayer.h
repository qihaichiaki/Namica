#pragma once

#include <namica/core/Layer.h>
#include <namica/renderer/EditorCamera.h>
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
    bool onWindowResize(WindowResizeEvent& _event);

private:
    EditorCamera m_editorCamera;
    Ref<Texture2D> m_testTetxure;
};

}  // namespace Namica