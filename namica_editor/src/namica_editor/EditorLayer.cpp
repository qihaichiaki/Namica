#include <namica_editor/EditorLayer.h>
#include <namica/core/Log.h>
#include <namica/core/Application.h>
#include <namica/core/Window.h>
#include <namica/renderer/Renderer.h>
#include <namica/renderer/Renderer2D.h>

#include <glm/gtc/matrix_transform.hpp>

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

}  // namespace Namica