#include "namica_editor/EditorLayer.h"
#include "namica/core/Log.h"

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

void EditorLayer::onUpdate()
{
    NAMICA_APP_DEBUG("编辑器层正在被更新......");
}

}  // namespace Namica