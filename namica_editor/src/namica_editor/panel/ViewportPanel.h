#pragma once

#include "namica_editor/panel/EditorPanel.h"
#include <glm/glm.hpp>

namespace Namica
{

class ViewportPanel final : public EditorPanel
{
public:
    ViewportPanel(EditorContext* _context);
    ~ViewportPanel();

    virtual void onUpdate() override;
    virtual void onImGuiRenderer() override;
    virtual void onEvent(Event& _event) override;

private:
    glm::vec2 m_viewportSize{0.0f, 0.0f};
    glm::vec2 m_viewportBounds[2]{};

    glm::vec2 m_lastViewportSize{0.f, 0.f};
    int m_stableFrames{0};

    bool m_isFocused{false};  // 鼠标聚焦到此面板上
    bool m_isHovered{true};   // 鼠标悬浮在此面板上
};

}  // namespace Namica