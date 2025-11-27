#include "namica_editor/panel/InfoPanel.h"
#include "namica_editor/EditorContext.h"
#include <namica/renderer/Renderer2D.h>
#include <imgui.h>

namespace Namica
{
InfoPanel::InfoPanel(EditorContext* _context) : EditorPanel{_context}
{
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::onImGuiRenderer()
{
    Renderer2D::Statistics const& renderer2DStats{Renderer2D::getStats()};
    ImGui::Begin("编辑器信息");
    ImGui::Text("Renderer2D状态");
    ImGui::Separator();
    ImGui::Text("绘制批次: %d", renderer2DStats.drawCalls);
    ImGui::Text("四边形个数: %d", renderer2DStats.quadCount);
    ImGui::Text("顶点个数: %d", renderer2DStats.getTotalVertexCount());
    ImGui::Text("索引个数: %d", renderer2DStats.getTotalIndexCount());
    ImGui::Text("线段个数: %d", renderer2DStats.lineCount);
    ImGui::Separator();

    ImGui::End();

    ImGui::Begin("编辑器设置");
    ImGui::Text("编辑器相机设置");
    ImGui::Separator();
    char const* projectionTypeStrings[] = {"透视", "正交"};
    char const* currentProjectionTypeString =
        projectionTypeStrings[static_cast<int>(m_context->editorCamera.getProjectionType())];
    if (ImGui::BeginCombo("投影类型", currentProjectionTypeString))
    {
        for (int i = 0; i < 2; ++i)
        {
            bool is_selected = currentProjectionTypeString == projectionTypeStrings[i];
            if (ImGui::Selectable(projectionTypeStrings[i], is_selected))
            {
                currentProjectionTypeString = projectionTypeStrings[i];
                m_context->editorCamera.setProjectionType(static_cast<Camera::ProjectionType>(i));
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    bool isRotation = m_context->editorCamera.isRotationEnabled();
    if (ImGui::Checkbox("旋转", &isRotation))
    {
        m_context->editorCamera.setRotationEnabled(isRotation);
    }
    ImGui::Separator();

    ImGui::Text("");
    ImGui::Text("编辑器热键");
    ImGui::Separator();
    ImGui::Text("移动: lalt+lmb");
    ImGui::Text("旋转: lalt+mmb");
    ImGui::Text("缩放: lalt+rmb | mouse scroll");
    ImGui::Text("复制选中实体: ctrl+d");
    ImGui::Separator();

    ImGui::End();
}

}  // namespace Namica