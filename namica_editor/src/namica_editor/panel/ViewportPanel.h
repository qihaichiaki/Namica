#pragma once

#include "namica_editor/panel/EditorPanel.h"

namespace Namica
{

class ViewportPanel final : public EditorPanel
{
public:
    ViewportPanel();
    ~ViewportPanel();

    virtual void onImGuiRenderer(EditorContext& _context) override;
};

}  // namespace Namica