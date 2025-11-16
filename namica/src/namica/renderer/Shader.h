#pragma once

#include "namica/core/Memory.h"
#include "namica/core/TypeMacros.h"

#include <glm/glm.hpp>
#include <string>

namespace Namica
{

class Shader
{
public:
    Shader() = default;
    virtual ~Shader() = default;

    /**
     * @brief shader设置4x4矩阵统一变量
     *
     * @param _name 变量名
     * @param _value Mat4值
     */
    virtual void setMat4(std::string const& _name, glm::mat4 const& _value) = 0;

    /**
     * @brief shader设置4维坐标统一变量
     *
     * @param _name 变量名
     * @param _value vec4值
     */
    virtual void setFloat4(std::string const& _name, glm::vec4 const& _value) = 0;

    /**
     * @brief shader设置整数统一变量
     *
     * @param _name 统一变量名
     * @param _value int值
     * @note opengl中,可为shader中的纹理采样器绑定纹理槽
     */
    virtual void setInt(std::string const& _name, int _value) = 0;

    /**
     * @brief shader设置整数数组统一变量
     *
     * @param _name 统一变量名
     * @param _value 数组地址
     * @param _count 数组个数
     */
    virtual void setIntArray(std::string const& _name, int* _value, uint32_t _count) = 0;

    /**
     * @brief shader设置浮点数统一变量
     *
     * @param _name 统一变量名
     * @param _value float值
     */
    virtual void setFloat(std::string const& _name, float _value) = 0;

public:
    /**
     * @brief 通过文件读入shader, 注意此shader文件需要满足存在顶点shader和片段shader
     *
     * @note ``#type``为shader文件标识, 具体分类为: ``vertex``(顶点shader)/``fragment`` or
     * ``pixel``(片段着色器)
     *
     * @param _filePath shader文件路径
     * @return Ref<Shader> Shader对象
     */
    static Ref<Shader> create(std::string_view _filePath);

    CLASS_DISABLE_COPY_MOVE(Shader)
};

}  // namespace Namica