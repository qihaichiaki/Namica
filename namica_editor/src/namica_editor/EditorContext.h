#pragma once

#include <namica/core/Memory.h>
#include <namica/renderer/EditorCamera.h>
#include <namica/scene/Scene.h>

namespace Namica
{

struct EditorContext
{
    EditorCamera editorCamera;

    Ref<Scene> activeScene{nullptr};
    Ref<Scene> editorScene{nullptr};
};

}  // namespace Namica