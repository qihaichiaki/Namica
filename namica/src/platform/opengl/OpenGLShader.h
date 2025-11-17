#pragma once

#include "namica/renderer/Shader.h"
#include <string>
#include <unordered_map>

typedef unsigned int GLenum;
namespace Namica
{

class OpenGLShader final : public Shader
{
public:
    OpenGLShader(std::string_view _filePath);
    ~OpenGLShader();

    virtual void setMat4(std::string const& _name, glm::mat4 const& _value) override;
    virtual void setFloat4(std::string const& _name, glm::vec4 const& _value) override;
    virtual void setInt(std::string const& _name, int _value) override;
    virtual void setIntArray(std::string const& _name, int* _value, uint32_t _count) override;
    virtual void setFloat(std::string const& _name, float _value) override;

private:
    // 将整个glsl文件依据shader类型拆分为可以编译的多个字符串
    std::unordered_map<GLenum, std::string> preProcess(std::string const& _source);
    // 将shader相关信息反射出来
    void reflect(GLenum stage, std::vector<uint32_t> const& _shaderData);
    // 将shader信息编译为Vulkan二进制信息
    void compileOrGetVulkanBinaries(std::unordered_map<GLenum, std::string> const& _shaderSources);
    // 将vulkan交叉编译为opengl二进制信息
    void compileOrGetOpenGLBinaries();
    // 创建shader程序
    void createProgram();
    // 是否切换program
    void useProgram();

private:
    uint32_t m_rendererID;
    std::string m_filePath;
    std::string m_shaderFileHash;

    std::unordered_map<GLenum, std::vector<uint32_t>> m_vulkanSPIRV;
    std::unordered_map<GLenum, std::vector<uint32_t>> m_openglSPIRV;
    std::unordered_map<GLenum, std::string> m_openglSourceCode;

    static uint32_t s_currentProgram;

    friend class OpenGLRendererAPI;
};

}  // namespace Namica