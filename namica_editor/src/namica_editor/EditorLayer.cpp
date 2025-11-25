#include <namica_editor/EditorLayer.h>
#include <namica/core/Log.h>
#include <namica/core/Application.h>
#include <namica/core/Window.h>
#include <namica/renderer/Renderer.h>
#include <namica/renderer/Renderer2D.h>
#include <namica/scene/Entity.h>

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

    // 场景相关初始化
    m_context.editorScene = Scene::create();
    m_context.activeScene = m_context.editorScene;

    // ui 初始化
    m_mainUI.editorPanelInit(&m_context);

    // 其他初始化
    m_testTetxure = Texture2D::create("assets/textures/namica.png");

    // Debug
    Entity testEntity1{m_context.activeScene->createEntity("测试矩形")};
    testEntity1.addComponent<SpriteRendererComponent>();

    Entity testEntity2{m_context.activeScene->createEntity("测试纹理")};
    testEntity2.addComponent<SpriteRendererComponent>().texture = m_testTetxure;
    testEntity2.getComponent<TransformComponent>().translation = glm::vec3{2.0f, 0.0f, 0.0f};

    Entity testEntity3{m_context.activeScene->createEntity("测试圆形")};
    testEntity3.addComponent<CircleRendererComponent>().color = glm::vec4{1.0f, 0.0f, 0.0f, 1.0f};
    testEntity3.getComponent<TransformComponent>().translation = glm::vec3{0.0f, -2.0f, 0.0f};
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

    // 场景更新和渲染更新
    Renderer2D::resetStats();
    m_context.activeScene->onUpdateEditor(0.0f, m_context.editorCamera);
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