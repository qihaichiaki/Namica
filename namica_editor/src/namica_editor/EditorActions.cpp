#include "namica_editor/EditorActions.h"
#include "namica_editor/EditorContext.h"

#include <namica/core/Log.h>

namespace Namica
{

EditorContext* EditorActions::s_context{nullptr};

void EditorActions::init(EditorContext* _context)
{
    s_context = _context;
    NAMICA_APP_ASSERT(s_context);
}

void EditorActions::duplicateEntity()
{
    if (s_context->selectionContext)
    {
        auto copyEntity = s_context->activeScene->copyEntity(s_context->selectionContext);
        copyEntity.getName() = std::format("copy-{}", copyEntity.getName());
        if (copyEntity == s_context->selectionContext ||
            copyEntity.getUUID() == s_context->selectionContext.getUUID())
        {
            NAMICA_CORE_ASSERT(false);
        }
    }
}

void EditorActions::enterPalyMode()
{
    if (s_context->sceneState == SceneState::Editor)
    {
        s_context->selectionContext = {};
        s_context->activeScene = Scene::copy(s_context->editorScene);
        s_context->activeScene->onStartRuntime();
        s_context->sceneState = SceneState::Runtime;
    }
}

void EditorActions::exitPalyMode()
{
    if (s_context->sceneState == SceneState::Runtime)
    {
        s_context->selectionContext = {};
        s_context->activeScene->onStopRuntime();
        s_context->activeScene = s_context->editorScene;
        s_context->sceneState = SceneState::Editor;
    }
}

}  // namespace Namica