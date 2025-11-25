#pragma once

#include "namica_editor/panel/EditorPanel.h"
#include <namica/core/UUID.h>

namespace Namica
{

class Entity;

class SceneHierarchyPanel : public EditorPanel
{
public:
    SceneHierarchyPanel(EditorContext* _context);
    ~SceneHierarchyPanel();

    virtual void onImGuiRenderer() override;

private:
    void drawEntityNode(Entity _entity);

private:
    UUID m_expandNextFrame{0};
};

}  // namespace Namica