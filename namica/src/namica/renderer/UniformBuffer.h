#pragma once

#include "namica/core/Memory.h"
#include "namica/core/TypeMacros.h"

namespace Namica
{

class UniformBuffer
{
public:
    UniformBuffer() = default;
    virtual ~UniformBuffer() = default;

    /**
     * @brief 设置统一变量缓冲区数据
     *
     * @param _data 实际数据
     * @param _size 缓冲区大小
     * @param _offset 偏移量
     */
    virtual void setData(void const* _data, uint32_t _size, uint32_t _offset = 0) = 0;

public:
    /**
     * @brief 创建统一变量的缓冲区, 用于shader中共享uniform数据的地方
     *
     * @param _size 缓冲区的大小
     * @param _binding UBO的绑定index, shader中需要指定的index
     * @return Ref<UniformBuffer>
     */
    static Ref<UniformBuffer> create(uint32_t _size, uint32_t _binding);

    CLASS_DISABLE_COPY_MOVE(UniformBuffer)
};

}  // namespace Namica