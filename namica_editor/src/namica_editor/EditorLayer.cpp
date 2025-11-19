#include <namica_editor/EditorLayer.h>
#include <namica/core/Log.h>
#include <namica/core/Application.h>
#include <namica/core/Window.h>
#include <namica/renderer/Renderer.h>
#include <namica/renderer/Renderer2D.h>

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

namespace Namica
{

EditorLayer::EditorLayer() : Layer("editor_layer")
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::onAttach()
{
    NAMICA_APP_DEBUG("编辑器层被添加");

    Renderer::setClearColor(glm::vec4{0.2f, 0.2f, 0.2f, 1.0f});

    m_editorCamera.setProjectionType(Camera::ProjectionType::Perspection);
    Window const& mainWindw{Application::get().getMainWindow()};
    m_editorCamera.updateViewportSize(mainWindw.getWidth(), mainWindw.getHeight());

    m_testTetxure = Texture2D::create("assets/textures/namica.png");
}

void EditorLayer::onDetach()
{
    NAMICA_APP_DEBUG("编辑器层被移除");
}

static bool s_init{false};

void EditorLayer::onUpdate()
{
    if (!s_init)
    {
        NAMICA_APP_DEBUG("编辑器层开始更新......");
        s_init = true;
    }

    m_editorCamera.onUpdate(0.0f);

    // renderer
    Renderer::clear();
    Renderer2D::resetStats();

    Renderer2D::beginScene(m_editorCamera.getProjectionView());

    Renderer2D::drawQuad(glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.0f, 0.0f}) *
                             glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f, 1.0f, 1.0f}),
                         glm::vec4{1.f, 1.f, 1.f, 1.0f});
    Renderer2D::drawQuad(glm::translate(glm::mat4{1.0f}, glm::vec3{2.0f, 0.0f, 0.0f}) *
                             glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f, 1.0f, 1.0f}),
                         m_testTetxure,
                         glm::vec4{1.f, 1.f, 1.f, 1.0f});

    Renderer2D::drawCircle(glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, -2.0f, 0.0f}) *
                               glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f, 1.0f, 1.0f}),
                           glm::vec4{1.0f, 0.0f, 0.0f, 1.0f},
                           1.0f);

    Renderer2D::drawRect(glm::translate(glm::mat4{1.0f}, glm::vec3{2.0f, -2.0f, 0.0f}) *
                             glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f, 1.0f, 1.0f}),
                         glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});

    Renderer2D::endScene();
}

void EditorLayer::onEvent(Event& _event)
{
    EventDispatcher dispatcher{_event};
    dispatcher.dispatch<WindowResizeEvent>(
        [this](WindowResizeEvent& _event) { return this->onWindowResize(_event); });

    m_editorCamera.onEvent(_event);
}

bool EditorLayer::onWindowResize(WindowResizeEvent& _event)
{
    uint32_t const width{_event.getWidth()};
    uint32_t const height{_event.getHeight()};

    m_editorCamera.updateViewportSize(width, height);
    Renderer::updateViewport(width, height);
    return false;
}

void EditorLayer::onImGuiRender()
{
    Renderer2D::Statistics const& renderer2DStats{Renderer2D::getStats()};
    ImGui::Begin("渲染信息");

    ImGui::Text("绘制批次: %d", renderer2DStats.drawCalls);
    ImGui::Text("四边形个数: %d", renderer2DStats.quadCount);
    ImGui::Text("顶点个数: %d", renderer2DStats.getTotalVertexCount());
    ImGui::Text("索引个数: %d", renderer2DStats.getTotalIndexCount());
    ImGui::Text("线段个数: %d", renderer2DStats.lineCount);

    ImGui::End();

    ImGui::Begin("编辑器设置");

    ImGui::Separator();
    ImGui::Text("编辑器相机设置");
    char const* projectionTypeStrings[] = {"透视", "正交"};
    char const* currentProjectionTypeString =
        projectionTypeStrings[static_cast<int>(m_editorCamera.getProjectionType())];
    if (ImGui::BeginCombo("投影类型", currentProjectionTypeString))
    {
        for (int i = 0; i < 2; ++i)
        {
            bool is_selected = currentProjectionTypeString == projectionTypeStrings[i];
            if (ImGui::Selectable(projectionTypeStrings[i], is_selected))
            {
                currentProjectionTypeString = projectionTypeStrings[i];
                m_editorCamera.setProjectionType(static_cast<Camera::ProjectionType>(i));
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    bool isRotation = m_editorCamera.isRotationEnabled();
    if (ImGui::Checkbox("旋转", &isRotation))
    {
        m_editorCamera.setRotationEnabled(isRotation);
    }
    ImGui::Text("移动: lalt+mmb");
    ImGui::Text("旋转: lalt+lmb");
    ImGui::Text("缩放: lalt+rmb | mouse scroll");

    ImGui::End();
}

}  // namespace Namica