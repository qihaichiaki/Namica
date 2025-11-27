#pragma once

#include <namica/core/Memory.h>
#include <namica/renderer/EditorCamera.h>
#include <namica/scene/Scene.h>
#include <namica/scene/Entity.h>

namespace Namica
{

enum class SceneState
{
    Editor,
    Runtime
};

struct EditorContext
{
    EditorCamera editorCamera;

    Ref<Scene> activeScene{nullptr};
    Ref<Scene> editorScene{nullptr};

    Entity selectionContext{};

    SceneState sceneState{SceneState::Editor};
};

}  // namespace Namica