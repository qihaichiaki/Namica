#include "namica_editor/EditorLayer.h"
#include "namica/core/Log.h"
#include "namica/renderer/Renderer.h"

namespace Namica
{

EditorLayer::EditorLayer() : Layer("editor_layer")
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::onAttach()
{
    NAMICA_APP_DEBUG("编辑器层被添加");

    Renderer::setClearColor(glm::vec4{0.2f, 0.2f, 0.2f, 1.0f});
}

void EditorLayer::onDetach()
{
    NAMICA_APP_DEBUG("编辑器层被移除");
}

static bool s_init{false};

void EditorLayer::onUpdate()
{
    if (!s_init)
    {
        NAMICA_APP_DEBUG("编辑器层开始更新......");
        s_init = true;
    }

    Renderer::clear();
}

}  // namespace Namica