#include "namica/renderer/EditorCamera.h"
#include "namica/core/Input.h"
#include "namica/events/MouseEvent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Namica
{

glm::quat EditorCamera::getOrientation() const
{
    return glm::quat{glm::vec3{-m_pitch, -m_yaw, 0.0f}};
}

glm::vec3 EditorCamera::getUpDirection() const
{
    return glm::rotate(getOrientation(), glm::vec3{0.0f, 1.0f, 0.0f});
}

glm::vec3 EditorCamera::getRightDirection() const
{
    return glm::rotate(getOrientation(), glm::vec3{1.0f, 0.0f, 0.0f});
}

glm::vec3 EditorCamera::getForwardDirection() const
{
    return glm::rotate(getOrientation(), glm::vec3{0.0f, 0.0f, -1.0f});
}

glm::vec3 EditorCamera::calculatePosition() const
{
    return m_focalPoint - getForwardDirection() * m_distance;
}

void EditorCamera::updateView()
{
    // m_yaw = m_pitch = 0.0f; // 锁住相机的旋转
    m_position = calculatePosition();

    glm::quat orientation = getOrientation();
    m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
    m_viewMatrix = glm::inverse(m_viewMatrix);
}

std::pair<float, float> EditorCamera::panSpeed() const
{
    float x = std::min(m_viewportWidth / 1000.0f, 2.4f);  // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(m_viewportHeight / 1000.0f, 2.4f);  // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {xFactor, yFactor};
}

float EditorCamera::rotationSpeed() const
{
    return 0.8f;
}

float EditorCamera::zoomSpeed() const
{
    float distance = m_distance * 0.2f;
    distance = std::max(distance, 0.0f);
    float speed = distance * distance;
    speed = std::min(speed, 100.0f);  // max speed = 100
    return speed;
}

void EditorCamera::mousePan(glm::vec2 const& _delta)
{
    auto [xSpeed, ySpeed] = panSpeed();
    m_focalPoint += -getRightDirection() * _delta.x * xSpeed * m_distance;
    m_focalPoint += getUpDirection() * _delta.y * ySpeed * m_distance;
}

void EditorCamera::mouseRotate(glm::vec2 const& _delta)
{
    float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
    m_yaw += yawSign * _delta.x * rotationSpeed();
    m_pitch += _delta.y * rotationSpeed();
}

void EditorCamera::mouseZoom(float _delta)
{
    m_distance -= _delta * zoomSpeed();
    if (m_distance < 1.0f)
    {
        m_focalPoint += getForwardDirection();
        m_distance = 1.0f;
    }
}

void EditorCamera::onUpdate(Timestep _ts)
{
    if (Input::isKeyPressed(KeyCode::LeftAlt))
    {
        glm::vec2 const& mouse{Input::getMouseX(), Input::getMouseY()};
        glm::vec2 delta = (mouse - m_initialMousePosition) * 0.003f;
        m_initialMousePosition = mouse;

        if (Input::isMouseButtonPressed(KeyCode::MouseButtonMiddle))
            mousePan(delta);
        else if (Input::isMouseButtonPressed(KeyCode::MouseButtonLeft))
            mouseRotate(delta);
        else if (Input::isMouseButtonPressed(KeyCode::MouseButtonRight))
            mouseZoom(delta.y);
    }
    updateView();
}

void EditorCamera::onEvent(Event& _event)
{
    EventDispatcher dispatcher(_event);
    dispatcher.dispatch<MouseScrolledEvent>(
        [this](MouseScrolledEvent& e) { return this->onMouseScroll(e); });
}

bool EditorCamera::onMouseScroll(MouseScrolledEvent& _mouseScrolledEvent)
{
    float delta = _mouseScrolledEvent.getOffsetY() * 0.1f;
    mouseZoom(delta);
    updateView();
    return false;
}

float EditorCamera::getDistance() const
{
    return m_distance;
}

void EditorCamera::setDistance(float _distance)
{
    m_distance = _distance;
}

glm::vec3 const& EditorCamera::getPosition() const
{
    return m_position;
}

float EditorCamera::getPitch() const
{
    return m_pitch;
}

float EditorCamera::getYaw() const
{
    return m_yaw;
}

glm::mat4 const& EditorCamera::getViewMatrix() const
{
    return m_viewMatrix;
}

glm::mat4 EditorCamera::getProjectionView() const
{
    return m_projection * m_viewMatrix;
}

}  // namespace Namica