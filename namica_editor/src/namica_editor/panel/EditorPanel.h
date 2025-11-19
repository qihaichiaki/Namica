#pragma once

namespace Namica
{

struct EditorContext;

class EditorPanel
{
public:
    virtual ~EditorPanel() = default;

    virtual void onImGuiRenderer(EditorContext& _context) = 0;
};

}  // namespace Namica