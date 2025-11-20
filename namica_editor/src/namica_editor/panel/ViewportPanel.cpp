#include "namica_editor/panel/ViewportPanel.h"
#include "namica_editor/EditorContext.h"

#include <namica/renderer/Renderer.h>
#include <imgui.h>

namespace Namica
{

ViewportPanel::ViewportPanel(EditorContext* _context) : EditorPanel{_context}
{
}

ViewportPanel::~ViewportPanel()
{
}

void ViewportPanel::onUpdate()
{
    // 编辑器相机更新
    if (m_isHovered)
    {
        m_context->editorCamera.onUpdate(0.0f);
    }

    // 解决视口频繁改动大小导致的闪屏问题
    if (m_viewportSize != m_lastViewportSize)
    {
        m_stableFrames = 0;
        m_lastViewportSize = m_viewportSize;
    }
    else
    {
        if (m_stableFrames <= 2)
        {
            m_stableFrames++;
        }

        if (m_stableFrames >= 2 && m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
            (static_cast<uint32_t>(m_viewportSize.x) != m_context->framebuffer->getWidth() ||
             static_cast<uint32_t>(m_viewportSize.y) != m_context->framebuffer->getHeight()))
        {
            uint32_t const viewWidth{static_cast<uint32_t>(m_viewportSize.x)};
            uint32_t const viewHeight{static_cast<uint32_t>(m_viewportSize.y)};
            m_context->editorCamera.updateViewportSize(viewWidth, viewHeight);
            Renderer::updateViewport(viewWidth, viewHeight);
        }
    }
}

void ViewportPanel::onImGuiRenderer()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
    ImGui::Begin("视口");
    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    m_viewportBounds[0] = {viewportMinRegion.x + viewportOffset.x,
                           viewportMinRegion.y + viewportOffset.y};
    m_viewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x,
                           viewportMaxRegion.y + viewportOffset.y};

    // 检查当前窗口是否是聚焦和悬浮的
    m_isFocused = ImGui::IsWindowFocused();
    m_isHovered = ImGui::IsWindowHovered();

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_viewportSize = {viewportPanelSize.x, viewportPanelSize.y};

    // 显示视图. v方向上反转一下
    ImGui::Image((void*)(uintptr_t)(m_context->framebuffer->getColorAttachmentRendererID(0)),
                 ImVec2{m_viewportSize.x, m_viewportSize.y},
                 ImVec2{0, 1},
                 ImVec2{1, 0});  // v方向反转一下

    ImGui::End();
    ImGui::PopStyleVar();
}

void ViewportPanel::onEvent(Event& _event)
{
    if (m_isHovered)
    {
        m_context->editorCamera.onEvent(_event);
    }
}

}  // namespace Namica