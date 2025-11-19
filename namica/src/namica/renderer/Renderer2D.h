#pragma once

#include "namica/core/Base.h"
#include "namica/core/Memory.h"
#include "namica/core/TypeMacros.h"
#include <glm/glm.hpp>

namespace Namica
{

struct Renderer2DConfig;
class Texture2D;
class NAMICA_API Renderer2D
{
public:
    // Stats
    struct NAMICA_API Statistics
    {
        uint32_t drawCalls = 0;
        uint32_t quadCount = 0;
        uint32_t lineCount = 0;

        uint32_t getTotalVertexCount() const
        {
            return quadCount * 4;
        }
        uint32_t getTotalIndexCount() const
        {
            return quadCount * 6;
        }
    };

public:
    /**
     * @brief 提交根据transform渲染的带颜色的四边形
     *
     * @param _transform 当前图形的基于世界的transform
     * @param _color 图形颜色
     * @param _entityID 实体id, 默认为-1, 表示无实体绘制. 编辑器特供
     */
    static void drawQuad(glm::mat4 const& _transform, glm::vec4 const& _color, int _entityID = -1);

    /**
     * @brief 提交根据transform渲染的带颜色, 平铺因子的纹理四边形
     *
     * @param _transform 当前纹理的基于世界的transform
     * @param _texture 2d纹理对象
     * @param _tintColor 混合颜色
     * @param _tilingFactor 平铺因子
     * @param _entityID 实体id, 默认为-1, 表示无实体绘制. 编辑器特供, 临时的
     */
    static void drawQuad(glm::mat4 const& _transform,
                         Ref<Texture2D> const& _texture,
                         glm::vec4 const& _tintColor = glm::vec4{1.0f},
                         float _tilingFactor = 1.0f,
                         int _entityID = -1);

    /**
     * @brief 提交绘制圆形
     *
     * @param _transform 圆形的坐标, 旋转, 缩放
     * @param _color 颜色
     * @param _thickness 圆的厚度 [0, 1.0f](1为实心圆, 0什么都没有)
     * @param _fade 模糊程度(0.005为最清晰)
     * @param _entityID 实体id, 默认为-1, 表示无实体绘制. 编辑器特供
     */
    static void drawCircle(glm::mat4 const& _transform,
                           glm::vec4 const& _color,
                           float _thickness,
                           float _fade = 0.005f,
                           int _entityID = -1);

    /**
     * @brief 绘制线段
     *
     * @param _p0 起点
     * @param _p1 终点
     * @param _color 线段颜色
     * @param _entityID 实体id, 默认为-1, 表示无实体绘制. 编辑器特供
     */
    static void drawLine(glm::vec3 const& _p0,
                         glm::vec3 const& _p1,
                         glm::vec4 const& _color,
                         int _entityID = -1);

    /**
     * @brief 绘制轴对齐矩形边框, 注意是垂直于z轴
     *
     * @param _position 中心点
     * @param _halfSize 半边宽高
     * @param _color 边框颜色
     * @param _entityID 实体id, 默认为-1, 表示无实体绘制. 编辑器特供
     */
    static void drawRect(glm::vec3 const& _position,
                         glm::vec2 const& _halfSize,
                         glm::vec4 const& _color,
                         int _entityID = -1);

    /**
     * @brief 绘制矩形边框
     *
     * @param _transform 矩形边框的变换
     * @param _color 矩形边框的颜色
     * @param _entityID 实体id, 默认为-1, 表示无实体绘制. 编辑器特供
     */
    static void drawRect(glm::mat4 const& _transform, glm::vec4 const& _color, int _entityID = -1);

    /// @brief 获取当前渲染器的调用状态
    static Statistics getStats();

    /// @brief 重置当前渲染器的调用状态
    static void resetStats();

private:
    /// @brief Renderer2D初始化准备
    static void init(Renderer2DConfig);

    /// @brief 2d渲染器释放资源, 一些申请的gpu的资源进行释放
    static void shutdown();

    /**
     * @brief 开始2D渲染场景, 此处需要上传相机的pv矩阵
     *
     * @param _pv 投影矩阵 * 视图矩阵
     */
    static void beginScene(glm::mat4 const& _pv);

    /**
     * @brief 停止2D渲染场景, 将内容进行渲染
     */
    static void endScene();

    /// @brief 绘制并且重新刷新渲染状态
    static void flushAndReset();

    CLASS_DISABLE_COPY_MOVE(Renderer2D)
    friend class Renderer;
};

}  // namespace Namica