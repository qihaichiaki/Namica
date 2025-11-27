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

}  // namespace Namica