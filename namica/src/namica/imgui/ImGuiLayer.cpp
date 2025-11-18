#include "namica/imgui/ImGuiLayer.h"
#include "namica/imgui/ImGuiImpl.h"
#include "namica/core/Application.h"
#include "namica/core/Window.h"
#include "namica/events/Event.h"
#include "namica/renderer/RendererContext.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace Namica
{
ImGuiLayer::ImGuiLayer() : Layer("ImGui")
{
}

ImGuiLayer::~ImGuiLayer()
{
}

void ImGuiLayer::onAttach()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGui后端IO设置
    auto& io = ImGui::GetIO();
    // 添加粗体
    io.Fonts->AddFontFromFileTTF(
        "assets/fonts/msyhbd.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    // 默认字体
    io.FontDefault = io.Fonts->AddFontFromFileTTF(
        "assets/fonts/msyh.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    auto& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    setDarkThemeColors();  // 设置默认风格、

    // 窗口和renderer后端初始化
    // TODO: 需要考虑下是否存在此layer重复被push的操作
    if (m_imguiImplWindow == nullptr)
    {
        m_imguiImplWindow = ImGuiImplWindow::create();
        m_imguiImplWindow->init(Application::get().getMainWindow());
    }
    if (m_imGuiImplRenderer == nullptr)
    {
        m_imGuiImplRenderer = ImGuiImplRenderer::create();
        m_imGuiImplRenderer->init();
    }
}

void ImGuiLayer::onDetach()
{
    m_imGuiImplRenderer->shutdown();
    m_imguiImplWindow->shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::onEvent(Event& _event)
{
    if (m_blockEvents)
    {
        // 屏蔽当imgui对鼠标和键盘进行捕获时, 确保不会传递下去
        ImGuiIO& io = ImGui::GetIO();
        _event.isHandled |= _event.isInCategory(CategoryMouse) & io.WantCaptureMouse;
        _event.isHandled |= _event.isInCategory(CategoryKeyboard) & io.WantCaptureKeyboard;
    }
}

void ImGuiLayer::onImGuiRender()
{
    // static bool is_show_demo = true;
    // ImGui::ShowDemoWindow(&is_show_demo);
}

void ImGuiLayer::setBlockEvents(bool _blockEvents)
{
    m_blockEvents = _blockEvents;
}

void ImGuiLayer::setDarkThemeColors()
{
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
}

void ImGuiLayer::begin()
{
    m_imguiImplWindow->newFrame();
    m_imGuiImplRenderer->newFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end()
{
    // 更新窗口大小
    auto& io = ImGui::GetIO();
    auto& window = Application::get().getMainWindow();
    io.DisplaySize = ImVec2{(float)window.getWidth(), (float)window.getHeight()};

    // imgui提交渲染任务
    ImGui::Render();
    // 后端实际使用执行绘制任务
    m_imGuiImplRenderer->drawData(ImGui::GetDrawData());

    // 只在开启多视口时执行
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        RendererContext& rendererContext{window.getRendererContext()};
        void* backup_current_context = rendererContext.getCurrentNativeHandle();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        rendererContext.makeCurrent(backup_current_context);
    }
}

}  // namespace Namica

ImGuiContext* GetImGuiContextPtr()
{
    return GImGui;
}