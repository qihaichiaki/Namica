#pragma once

#include "namica/core/Memory.h"
#include "namica/core/TypeMacros.h"

namespace Namica
{

class VertexBuffer;
class IndexBuffer;

class VertexArray
{
public:
    VertexArray() = default;
    virtual ~VertexArray() = default;

    /**
     * @brief 顶点数组对象设置顶点缓冲区
     *
     * @param _vertexBuffer 顶点缓冲区
     */
    virtual void setVertexBuffer(Ref<VertexBuffer> const& _vertexBuffer) = 0;

    /**
     * @brief 顶点数组对象设置索引缓冲区
     *
     * @param _indexBuffer 索引缓冲区
     */
    virtual void setIndexBuffer(Ref<IndexBuffer> const& _indexBuffer) = 0;

public:
    Ref<VertexArray> create();

    CLASS_DISABLE_COPY_MOVE(VertexArray)
};

}  // namespace Namica