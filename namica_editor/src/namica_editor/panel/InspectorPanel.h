#pragma once

#include "namica_editor/panel/EditorPanel.h"

namespace Namica
{

class Entity;

class InspectorPanel : public EditorPanel
{
public:
    InspectorPanel(EditorContext* _context);
    ~InspectorPanel();

    virtual void onImGuiRenderer() override;

private:
    void drawComponents(Entity);
};

}  // namespace Namica