#include "namica_editor/EditorLayer.h"
#include "namica_editor/EditorActions.h"

#include <namica/core/Log.h>
#include <namica/core/Application.h>
#include <namica/core/Window.h>
#include <namica/core/Input.h>
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
    // 默认2D引擎
    m_context.editorCamera.setProjectionType(Camera::ProjectionType::Orthographic);
    m_context.editorCamera.setRotationEnabled(false);

    // 场景相关初始化
    m_context.editorScene = Scene::create();
    Entity runtimeCamera{m_context.editorScene->createEntity("相机")};
    runtimeCamera.addComponent<CameraComponent>();
    m_context.editorScene->setDrawColliders2D(true);

    m_context.activeScene = m_context.editorScene;

    // 场景行为初始化
    EditorActions::init(&m_context);

    // ui 初始化
    m_mainUI.editorPanelInit(&m_context);

    // 其他初始化
    m_testTetxure = Texture2D::create("assets/textures/namica.png");

    // // Debug
    // Entity testEntity1{m_context.activeScene->createEntity("测试矩形")};
    // testEntity1.addComponent<SpriteRendererComponent>();

    // Entity testEntity2{m_context.activeScene->createEntity("测试纹理")};
    // testEntity2.addComponent<SpriteRendererComponent>().texture = m_testTetxure;
    // testEntity2.getComponent<TransformComponent>().translation = glm::vec3{2.0f, 0.0f, 0.0f};

    // Entity testEntity3{m_context.activeScene->createEntity("测试圆形")};
    // testEntity3.addComponent<CircleRendererComponent>().color = glm::vec4{1.0f, 0.0f,
    // 0.0f, 1.0f}; testEntity3.getComponent<TransformComponent>().translation = glm::vec3{0.0f,
    // -2.0f, 0.0f};
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
    if (m_context.sceneState == SceneState::Editor)
    {
        m_context.activeScene->onUpdateEditor(0.0f, m_context.editorCamera);
    }
    else
    {
        m_context.activeScene->onUpdateRuntime(1.0f / 60.0f);
    }
}

void EditorLayer::onEvent(Event& _event)
{
    // 全局事件处理
    EventDispatcher dispatcher{_event};
    dispatcher.dispatch<KeyPressedEvent>(
        [this](KeyPressedEvent& _keyPressedEvent) { return onKeyPressed(_keyPressedEvent); });

    // 面板事件处理
    m_mainUI.onEvent(_event);
}

bool EditorLayer::onKeyPressed(KeyPressedEvent& _event)
{
    // 短按, 长按排除
    if (_event.getRepeatCount() > 0)
    {
        return false;
    }

    bool isCtrl{Input::isKeyPressed(KeyCode::LeftControl) ||
                Input::isKeyPressed(KeyCode::RightControl)};
    bool isShift{Input::isKeyPressed(KeyCode::LeftShift) ||
                 Input::isKeyPressed(KeyCode::RightShift)};

    switch (_event.getKeyCode())
    {
        case KeyCode::N:
            if (isCtrl)
            {
                // newScene();
            }
            break;
        case KeyCode::O:
            if (isCtrl)
            {
                // openScene();
            }
            break;
        case KeyCode::S:
            if (isCtrl)
            {
                if (isShift)
                {
                    // saveSceneAs();
                }
                else
                {
                    // saveScene();
                }
            }
            break;
        case KeyCode::D:
            if (isCtrl)
            {
                EditorActions::duplicateEntity();
            }
            break;
        case KeyCode::Q:
            // m_gizom_type = -1;
            break;
        case KeyCode::W:
            // m_gizom_type = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case KeyCode::E:
            // m_gizom_type = ImGuizmo::OPERATION::ROTATE;
            break;
        case KeyCode::R:
            // m_gizom_type = ImGuizmo::OPERATION::SCALE;
            break;
        default:
            break;
    }

    return false;
}

void EditorLayer::onImGuiRender()
{
    m_mainUI.drawDockspace();
    m_mainUI.drawPanels();
}

}  // namespace Namica