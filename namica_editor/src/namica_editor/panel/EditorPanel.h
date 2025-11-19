#pragma once

#include <namica/core/Log.h>
#include <namica/events/Event.h>

namespace Namica
{

struct EditorContext;

class EditorPanel
{
public:
    EditorPanel(EditorContext* _context) : m_context{_context}
    {
        NAMICA_APP_ASSERT(m_context);
    }
    virtual ~EditorPanel() = default;

    virtual void onUpdate()
    {
    }
    virtual void onImGuiRenderer() = 0;
    virtual void onEvent(Event& _event)
    {
    }

protected:
    EditorContext* m_context{nullptr};
};

}  // namespace Namica