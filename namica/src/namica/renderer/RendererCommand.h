#pragma once

#include "namica/core/Memory.h"
#include "namica/renderer/RendererAPI.h"
#include "namica/renderer/RendererConfig.h"

namespace Namica
{

class RendererCommand
{
public:
    /**
     * @brief 渲染API初始化
     */
    static void init(RendererAPIType _rendererAPIType);

    /**
     * @brief 返回当前渲染API的底层类型
     */
    static RendererAPIType getRendererAPIType() noexcept;

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
     * @brief 传入顶点数组对象, 绘制包含的所有索引顶点
     *
     * @param _vertexArray 顶点数组对象
     * @param _shader shader对象
     */
    static void drawIndexed(Ref<VertexArray> const& _vertexArray, Ref<Shader> const& _shader);

    /**
     * @brief 传入顶点数组对象, 绘制指定索引个数
     *
     * @param _vertexArray 顶点数组对象
     * @param _shader shader对象
     * @param _indexCount 指定绘制索引个数
     */
    static void drawIndexed(Ref<VertexArray> const& _vertexArray,
                            Ref<Shader> const& _shader,
                            uint32_t _indexCount);

    /**
     * @brief 传入顶点数组对象, 绘制指定顶点个数的线段
     *
     * @param _vertexArray 顶点数组对象
     * @param _shader shader对象
     * @param _vertexCount 指定线段绘制顶点个数
     */
    static void drawLines(Ref<VertexArray> const& _vertexArray,
                          Ref<Shader> const& _shader,
                          uint32_t _vertexCount);

    /**
     * @brief 设置通用绘制线段的宽度大小
     *
     * @param _width 线段宽度
     */
    static void setLineWidth(float _width);

    /**
     * @brief 更新渲染显示窗口大小
     *
     * @param _width 渲染宽度
     * @param _height 渲染高度
     */
    static void updateViewport(uint32_t _width, uint32_t _height);

private:
    static Scope<RendererAPI> s_rendererAPI;
    static RendererAPIType s_rendererAPIType;
};

}  // namespace Namica