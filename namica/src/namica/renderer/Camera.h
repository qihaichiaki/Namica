#pragma once

#include "namica/core/Base.h"
#include <glm/glm.hpp>

namespace Namica
{

class Camera
{
public:
    enum class ProjectionType
    {
        Perspection = 0,  // 透视
        Orthographic = 1  // 正交
    };

public:
    NAMICA_API Camera() = default;
    NAMICA_API virtual ~Camera() = default;

    /// @brief 设置相机的投影矩阵类型
    NAMICA_API void setProjectionType(ProjectionType _projectionType);
    /// @brief 返回相机的投影矩阵类型
    NAMICA_API ProjectionType getProjectionType() const;
    /// @brief 返回相机的投影矩阵
    NAMICA_API glm::mat4 const& getProjection() const;

    /// @brief 设置透视相机垂直视野的弧度值
    NAMICA_API void setPerspectiveFOV(float _perspectiveFOV);
    /// @brief 获取透视相机垂直视野的弧度值
    NAMICA_API float getPerspectiveFOV() const;
    /// @brief 设置透视相机裁剪空间近点值
    NAMICA_API void setPerspectiveNearClip(float _perspectiveNear);
    /// @brief 获取透视相机裁剪空间近点值
    NAMICA_API float getPerspectiveNearClip() const;
    /// @brief 设置透视相机裁剪空间远点值
    NAMICA_API void setPerspectiveFarClip(float _perspectiveFar);
    /// @brief 获取透视相机裁剪空间远点值
    NAMICA_API float getPerspectiveFarClip() const;
    /// @brief 设置正交相机范围大小
    NAMICA_API void setOrthographicSize(float _orthographicSize);
    /// @brief 获取正交相机范围大小
    NAMICA_API float getOrthographicSize() const;
    /// @brief 设置正交相机裁剪空间近点值
    NAMICA_API void setOrthographicNearClip(float _orthographicNear);
    /// @brief 获取正交相机裁剪空间近点值
    NAMICA_API float getOrthographicNearClip() const;
    /// @brief 设置正交相机裁剪空间远点值
    NAMICA_API void setOrthographicFarClip(float _orthographicFar);
    /// @brief 获取正交相机裁剪空间远点值
    NAMICA_API float getOrthographicFarClip() const;

    /// @brief 更新相机的可视宽度和高度
    NAMICA_API void updateViewportSize(uint32_t _width, uint32_t _height);

protected:
    // 根据当前类型和参数计算投影矩阵
    void recalculateProjection();

protected:
    // 投影类型, 因为是2d引擎, 所以优先正交投影
    ProjectionType m_projectionType{ProjectionType::Orthographic};
    glm::mat4 m_projection{1.0f};

    // 透视相关参数
    float m_perspectiveFOV{glm::radians(45.0f)};  // 垂直视场
    float m_perspectiveNear{0.01f};               // 近点
    float m_perspectiveFar{1000.0f};              // 远点

    // 正交相关参数
    float m_orthographicSize{10.0f};
    float m_orthographicNear{-1.0f};
    float m_orthographicFar{1.0f};

    float m_aspectRatio{0.0f};
    float m_viewportWidth{0.0f};
    float m_viewportHeight{0.0f};
};

}  // namespace Namica