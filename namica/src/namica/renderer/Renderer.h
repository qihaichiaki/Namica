#pragma once

#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include "namica/core/Memory.h"
#include <glm/glm.hpp>

namespace Namica
{

struct RendererConfig;
class Framebuffer;

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
     * @brief 开始渲染
     *
     * @note 注意Renderer/Renderer2D内的所有渲染的接口均需要在此接口调用后进行使用
     *
     */
    static void beginRender(glm::mat4 const& _cameraPV,
                            Ref<Framebuffer> const& _frameBuffer = nullptr);

    /**
     * @brief 终止渲染
     *
     */
    static void endRender();

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

    CLASS_DISABLE_COPY_MOVE(Renderer)
};

}  // namespace Namica