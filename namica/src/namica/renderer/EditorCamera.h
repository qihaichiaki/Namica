#pragma once

#include "namica/renderer/Camera.h"
#include "namica/core/Time.h"

namespace Namica
{

class Event;
class MouseScrolledEvent;

class EditorCamera final : public Camera
{
public:
    NAMICA_API EditorCamera() = default;
    NAMICA_API ~EditorCamera() = default;

    NAMICA_API void onUpdate(Timestep _ts);
    NAMICA_API void onEvent(Event& _event);

    NAMICA_API float getDistance() const;
    NAMICA_API void setDistance(float _distance);
    NAMICA_API glm::vec3 getUpDirection() const;
    NAMICA_API glm::vec3 getRightDirection() const;
    NAMICA_API glm::vec3 getForwardDirection() const;
    NAMICA_API const glm::vec3& getPosition() const;
    NAMICA_API glm::quat getOrientation() const;
    NAMICA_API float getPitch() const;
    NAMICA_API float getYaw() const;

    NAMICA_API const glm::mat4& getViewMatrix() const;
    NAMICA_API glm::mat4 getProjectionView() const;

private:
    glm::vec3 calculatePosition() const;
    void updateView();
    bool onMouseScroll(MouseScrolledEvent& _mouseScrolledEvent);
    void mousePan(glm::vec2 const& _delta);
    void mouseRotate(glm::vec2 const& _delta);
    void mouseZoom(float _delta);
    std::pair<float, float> panSpeed() const;
    float rotationSpeed() const;
    float zoomSpeed() const;

private:
    glm::mat4 m_viewMatrix{1.0f};
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_focalPoint = {0.0f, 0.0f, 0.0f};
    glm::vec2 m_initialMousePosition = {0.0f, 0.0f};
    float m_distance{10.0f};
    float m_pitch{0.0f};
    float m_yaw{0.0f};
};

}  // namespace Namica