#include "namica/renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Namica
{

void Camera::setProjectionType(ProjectionType _projectionType)
{
    if (m_projectionType != _projectionType)
    {
        m_projectionType = _projectionType;
        recalculateProjection();
    }
}

Camera::ProjectionType Camera::getProjectionType() const
{
    return m_projectionType;
}

glm::mat4 const& Camera::getProjection() const
{
    return m_projection;
}

void Camera::setPerspectiveFOV(float _perspectiveFOV)
{
    m_perspectiveFOV = _perspectiveFOV;
    recalculateProjection();
}

float Camera::getPerspectiveFOV() const
{
    return m_perspectiveFOV;
}

void Camera::setPerspectiveNearClip(float _perspectiveNear)
{
    m_perspectiveNear = _perspectiveNear;
    recalculateProjection();
}

float Camera::getPerspectiveNearClip() const
{
    return m_perspectiveNear;
}

void Camera::setPerspectiveFarClip(float _perspectiveFar)
{
    m_perspectiveFar = _perspectiveFar;
    recalculateProjection();
}

float Camera::getPerspectiveFarClip() const
{
    return m_perspectiveFar;
}

void Camera::setOrthographicSize(float _orthographicSize)
{
    m_orthographicSize = _orthographicSize;
    recalculateProjection();
}

float Camera::getOrthographicSize() const
{
    return m_orthographicSize;
}

void Camera::setOrthographicNearClip(float _orthographicNear)
{
    m_orthographicNear = _orthographicNear;
    recalculateProjection();
}

float Camera::getOrthographicNearClip() const
{
    return m_orthographicNear;
}

void Camera::setOrthographicFarClip(float _orthographicFar)
{
    m_orthographicFar = _orthographicFar;
    recalculateProjection();
}

float Camera::getOrthographicFarClip() const
{
    return m_orthographicFar;
}

void Camera::updateViewportSize(uint32_t _width, uint32_t _height)
{
    float aspectRatio{_width * 1.0f / _height};
    if (std::fabs(m_aspectRatio - aspectRatio) > 1e-3f)
    {
        m_aspectRatio = aspectRatio;
        m_viewportWidth = 1.0f * _width;
        m_viewportHeight = 1.0f * _height;
        recalculateProjection();
    }
}

void Camera::recalculateProjection()
{
    if (m_projectionType == ProjectionType::Perspection)
    {
        m_projection =
            glm::perspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
    }
    else
    {
        float ortho_left = -0.5f * m_orthographicSize * m_aspectRatio;
        float ortho_right = 0.5f * m_orthographicSize * m_aspectRatio;
        float ortho_bottom = -0.5f * m_orthographicSize;
        float ortho_top = 0.5f * m_orthographicSize;

        m_projection = glm::ortho(ortho_left,
                                  ortho_right,
                                  ortho_bottom,
                                  ortho_top,
                                  m_orthographicNear,
                                  m_orthographicFar);
    }
}

}  // namespace Namica