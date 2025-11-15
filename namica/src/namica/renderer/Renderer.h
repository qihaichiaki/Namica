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
     * @brief 设置清屏颜色
     *
     * @param _clearColor rgba
     */
    static void setClearColor(glm::vec4 const& _clearColor);

    /**
     * @brief 使用清屏颜色清屏
     */
    static void clear();

private:
    /**
     * @brief 渲染器的初始化
     */
    static void init(RendererConfig const& _rendererConfig);

    CLASS_DISABLE_COPY_MOVE(Renderer)
    friend class Application;
};

}  // namespace Namica