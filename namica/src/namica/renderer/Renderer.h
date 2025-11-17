#pragma once

#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include <glm/glm.hpp>

namespace Namica
{

struct RendererConfig;

class NAMICA_API Renderer
{
public:
    /**
     * @brief 渲染器的初始化
     */
    static void init(RendererConfig const& _rendererConfig);

    /**
     * @brief 渲染器的清理
     *
     */
    static void shutdown();

    /**
     * @brief 设置清屏颜色
     *
     * @param _clearColor rgba
     */
    static void setClearColor(glm::vec4 const& _clearColor);

    /**
     * @brief 使用清屏颜色清屏
     */
    static void clear();

    /**
     * @brief 更新渲染窗口大小
     *
     * @param _width 渲染宽度
     * @param _height 渲染高度
     */
    static void updateViewport(uint32_t _width, uint32_t _height);

private:
    CLASS_DISABLE_COPY_MOVE(Renderer)
};

}  // namespace Namica