#pragma once

#include "namica/core/Base.h"  // IWYU pragma: keep 使用了uint32_t
#include "namica/core/Memory.h"
#include "namica/core/TypeMacros.h"
#include <vector>

namespace Namica
{

/**
 * @brief 对应到shader中元素(相当于顶点的组成部分, 比如坐标, 纹理坐标...)的类型
 */
enum class ShaderDataType
{
    None,
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    Mat3,
    Mat4,
    Bool
};

// 每个顶点元素的结构组成
struct VertexElement
{
    ShaderDataType type{};  // 顶点元素类型, 方便计算偏移量和大小
    char const* name{""};   // 顶点元素名称,对应shader中接收元素的名字
    uint32_t offset{};      // 顶点元素在当前顶点中的偏移字节数(注意, 针对于shader程序)
    uint32_t size{};        // 当前顶点元素在shader中所占的字节大小
    bool normalized{};      // 当前顶点元素是否需要归一化

    VertexElement(ShaderDataType _type, char const* _name, bool _normalized = false);

    /**
     * @brief 返回当前顶点元素中数据的个数, 比如类型为Float2的元素个数存在2个，Mat3就是3*3=9个
     *
     * @return 顶点元素的数据个数
     */
    uint32_t getComponentCount() const noexcept;
};

// 顶点布局
class VertexLayout
{
public:
    VertexLayout() = default;
    VertexLayout(std::initializer_list<VertexElement> const& _elements);

    std::vector<VertexElement>::iterator begin();
    std::vector<VertexElement>::iterator end();
    std::vector<VertexElement>::const_iterator begin() const;
    std::vector<VertexElement>::const_iterator end() const;
    bool empty() const;
    uint32_t getStride() const;

private:
    std::vector<VertexElement> m_elements;
    uint32_t m_stride;  // 顶点元素的总偏移量
};

class VertexBuffer
{
public:
    VertexBuffer() = default;
    virtual ~VertexBuffer() = default;

    /**
     * @brief 往动态顶点缓冲区设置相应应用大小的数据
     *
     * @param _data 实际顶点数据缓冲区, 可以是由顶点属性聚合的struct数组传入
     * @param _size 实际应用的顶点数组大小
     */
    virtual void setData(void const* _data, uint32_t _size) = 0;

    /**
     * @brief 设置顶点缓冲区内每个顶点元素的布局
     *
     * @param _vertexLayout 顶点元素布局
     */
    virtual void setLayout(VertexLayout const& _vertexLayout) = 0;

    /**
     * @brief 获取顶点缓冲区内每个顶点元素布局
     *
     * @return VertexLayout const& 顶点元素布局
     */
    virtual VertexLayout const& getLayout() const noexcept = 0;

public:
    /**
     * @brief 创建静态顶点缓冲区, 此顶点缓冲区的内容永远不会发生变化
     *
     * @param _vertices 顶点数组
     * @param _size 顶点数组大小
     */
    static Ref<VertexBuffer> create(float* _vertices, uint32_t _size);

    /**
     * @brief 创建固定长度的动态顶点缓冲区, 其内容动态变化
     *
     * @param _size 动态顶点缓冲区的固定大小
     */
    static Ref<VertexBuffer> create(uint32_t _size);

    CLASS_DISABLE_COPY_MOVE(VertexBuffer)
};

}  // namespace Namica