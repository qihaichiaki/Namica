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

    // context 初始化
    m_context.editorCamera.setProjectionType(Camera::ProjectionType::Perspection);

    FramebufferConfig framebufferConfig{};
    // framebufferConfig.samples = 1;  // 1倍采样， 除开一倍采样其余均存在问题
    framebufferConfig.framebufferTextureAttachment = FramebufferTextureAttachment{
        FramebufferTextureFormat::RGBA8,            // 正常渲染的目标纹理
        FramebufferTextureFormat::RED_INTEGER,      // 需要为渲染对象添加的纹理标记ID
        FramebufferTextureFormat::DEPTH24_STENCIL8  // 深度缓冲区
    };
    m_context.framebuffer = Framebuffer::create(framebufferConfig);

    // ui 初始化
    m_mainUI.editorPanelInit(&m_context);

    // 其他初始化
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

    // 面板更新
    m_mainUI.onUpdate();

    // renderer
    Renderer::beginRender(m_context.editorCamera.getProjectionView(), m_context.framebuffer);

    Renderer::clear();
    m_context.framebuffer->clearAttachment(1, -1);  // 纹理附加信息 清理为-1

    Renderer2D::resetStats();

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

    Renderer::endRender();
}

void EditorLayer::onEvent(Event& _event)
{
    m_mainUI.onEvent(_event);
}

void EditorLayer::onImGuiRender()
{
    m_mainUI.drawDockspace();
    m_mainUI.drawPanels();
}

}  // namespace Namica