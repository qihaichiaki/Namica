#pragma once

#include "namica/core/Base.h"  // IWYU pragma: keep 使用了uint32_t
#include "namica/core/Memory.h"
#include "namica/core/TypeMacros.h"

namespace Namica
{

class IndexBuffer
{
public:
    IndexBuffer() = default;
    virtual ~IndexBuffer() = default;

    /**
     * @brief 返回索引缓冲区内的索引个数
     */
    virtual uint32_t getCount() const noexcept = 0;

public:
    /**
     * @brief 创建索引缓冲区
     *
     * @param _indices 索引数组
     * @param _count 索引个数
     */
    static Ref<IndexBuffer> create(uint32_t const* _indices, uint32_t _count);

    CLASS_DISABLE_COPY_MOVE(IndexBuffer)
};

}  // namespace Namica