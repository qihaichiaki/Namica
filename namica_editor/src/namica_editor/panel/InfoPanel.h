#pragma once

#include "namica_editor/panel/EditorPanel.h"

namespace Namica
{

class InfoPanel final : public EditorPanel
{
public:
    InfoPanel(EditorContext* _context);
    ~InfoPanel();

    virtual void onImGuiRenderer() override;
};

}  // namespace Namica