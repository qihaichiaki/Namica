#include "namica_editor/EditorMainUI.h"
#include "namica_editor/panel/ViewportPanel.h"
#include "namica_editor/panel/InfoPanel.h"

#include <imgui.h>

namespace Namica
{

void EditorMainUI::editorPanelInit(EditorContext* _context)
{
    m_panels.emplace_back(createRef<ViewportPanel>(_context));
    m_panels.emplace_back(createRef<InfoPanel>(_context));
}

void EditorMainUI::onUpdate()
{
    for (auto& panel : m_panels)
    {
        panel->onUpdate();
    }
}

void EditorMainUI::onEvent(Event& _event)
{
    for (auto& panel : m_panels)
    {
        panel->onEvent(_event);
    }
}

void EditorMainUI::drawDockspace()
{
    // dockspace 创建停靠区域
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static bool dockspace_open = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport const* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking,
    // otherwise any change of dockspace/settings would lead to windows being stuck in limbo and
    // never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float min_win_size_x = style.WindowMinSize.x;
    style.WindowMinSize.x = 380.0f;  // 窗口被停靠, 宽度至少为370
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = min_win_size_x;  // 还原
    drawMenuBar();
    ImGui::End();
}

void EditorMainUI::drawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("开始"))
        {
            if (ImGui::MenuItem("新建场景", "CTRL+N"))
            {
            }
            if (ImGui::MenuItem("打开场景...", "CTRL+O"))
            {
            }
            if (ImGui::MenuItem("另存为场景...", "CTRL+SHIFT+S"))
            {
            }
            if (ImGui::MenuItem("关闭"))
            {
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void EditorMainUI::drawPanels()
{
    for (auto& panel : m_panels)
    {
        panel->onImGuiRenderer();
    }
}

}  // namespace Namica