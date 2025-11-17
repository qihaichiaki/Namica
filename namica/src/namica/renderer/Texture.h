#pragma once

#include "namica/core/Base.h"
#include "namica/core/Memory.h"
#include "namica/core/TypeMacros.h"

#include <string>

namespace Namica
{

class NAMICA_API Texture
{
public:
    virtual ~Texture() = default;

    /**
     * @brief 纹理的宽度
     */
    virtual uint32_t getWidth() const = 0;

    /**
     * @brief 纹理的高度
     */
    virtual uint32_t getHeight() const = 0;

    /**
     * @brief 用户上传纹理相关数据到shader中
     *
     * @param slot 上传哪个纹理插槽,默认为0
     */
    virtual void bind(uint32_t _slot = 0) const = 0;

    /**
     * @brief 设置纹理数据上传到gpu上
     *
     * @param _data 纹理数据, 注意宽度和高度需要和当前纹理一致
     * @param _size 当前的纹理数据字节大小
     */
    virtual void setData(void* _data, uint32_t _size) = 0;

    /**
     * @brief 判断两个纹理是否一致
     */
    virtual bool isEqual(Texture const&) = 0;

    /**
     * @brief 返回底层的OpenGL渲染ID
     *
     * @warning 后续需要优化, 当前仅仅适用于opengl渲染
     */
    virtual uint32_t getRendererId() = 0;
};

class NAMICA_API Texture2D : public Texture
{
public:
    Texture2D() = default;
    virtual ~Texture2D() = default;

public:
    /**
     * @brief 创建2d纹理类型
     *
     * @param _width 纹理宽度
     * @param _height 纹理高度
     *
     * @note 创建完毕后, 通关纹理的设置数据即可, 注意数据大小要和宽度高度匹配, 单位为RGBA
     */
    static Ref<Texture2D> create(uint32_t _width, uint32_t _height);

    /**
     * @brief 创建2d纹理类型
     *
     * @param _path 图像路径(jpg/png)
     */
    static Ref<Texture2D> create(std::string const& _path);

    CLASS_DISABLE_COPY_MOVE(Texture2D)
};

}  // namespace Namica