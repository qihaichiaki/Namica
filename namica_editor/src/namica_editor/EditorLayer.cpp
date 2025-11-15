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

    Renderer::setClearColor({1., 1., 1., 1.});
    Renderer::clear();
}

}  // namespace Namica