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

    NAMICA_API const glm::mat4& getViewMatrix() const;
    NAMICA_API glm::mat4 getProjectionView() const;

    /// @brief 设置相机是否允许旋转
    NAMICA_API void setRotationEnabled(bool _enabled);
    /// @brief 查看相机是否允许旋转
    NAMICA_API bool isRotationEnabled() const;

private:
    float getDistance() const;
    void setDistance(float _distance);
    glm::vec3 getUpDirection() const;
    glm::vec3 getRightDirection() const;
    glm::vec3 getForwardDirection() const;
    glm::vec3 const& getPosition() const;
    glm::quat getOrientation() const;
    float getPitch() const;
    float getYaw() const;

    glm::vec3 calculatePosition() const;
    void updateView();
    bool onMouseScroll(MouseScrolledEvent& _mouseScrolledEvent);
    void mousePan(glm::vec2 const& _delta);
    void mouseRotate(glm::vec2 const& _delta);
    void mouseZoom(float _delta);
    std::pair<float, float> panSpeed() const;
    float rotationSpeed() const;
    float zoomSpeed() const;
    float orthoZoomSpeed() const;

private:
    glm::mat4 m_viewMatrix{1.0f};
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_focalPoint = {0.0f, 0.0f, 0.0f};
    glm::vec2 m_initialMousePosition = {0.0f, 0.0f};
    float m_distance{10.0f};
    float m_pitch{0.0f};
    float m_yaw{0.0f};
    bool m_isRotation{true};
};

}  // namespace Namica