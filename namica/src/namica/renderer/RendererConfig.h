#pragma once

#include "namica/core/Base.h"

namespace Namica
{

enum class RendererAPIType
{
    None,
    OpenGL
};

/**
 * @brief 2D渲染配置
 */
struct NAMICA_API Renderer2DConfig
{
    uint32_t maxQuads{10000};  // 允许一批绘制的最大四边形数量
    // TODO: 线条渲染当前是无上限的
};

/**
 * @brief 渲染配置
 */
struct NAMICA_API RendererConfig
{
    RendererAPIType rendererAPIType{RendererAPIType::None};
    Renderer2DConfig renderer2DConfig{};
};

}  // namespace Namica