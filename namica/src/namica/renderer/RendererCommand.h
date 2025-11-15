#pragma once

#include "namica/core/Memory.h"
#include "namica/renderer/RendererAPI.h"
#include "namica/renderer/RendererConfig.h"

namespace Namica
{

enum class RendererAPIType;

class RendererCommand
{
public:
    /**
     * @brief 渲染API初始化
     */
    static void init(RendererAPIType _rendererAPIType);

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
    static Scope<RendererAPI> s_rendererApi;
};

}  // namespace Namica