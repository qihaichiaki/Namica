#include "platform/opengl/OpenGLShader.h"
#include "namica/core/Log.h"
#include "namica/core/Timer.h"
#include "namica/utilities/FileSystem.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <filesystem>
#include <fstream>

namespace Namica
{

static char const* getCacheDirectory()
{
    // 固定位置, 未来可以支持缓存位置配置
    return "assets/cache/shader/opengl";
}

// 如果缓存目录不存在则进行创建
static void createCacheDirectoryIfNeeded()
{
    std::string cacheDirectory = getCacheDirectory();
    if (!FileSystem::exists(cacheDirectory))
    {
        NAMICA_CORE_ASSERT(FileSystem::createDirectories(cacheDirectory));
    }
}

// 返回对应shader生成缓存文件的后缀名 - vulkan类型
static char const* glShaderStageCachedVulkanFileExtension(uint32_t _stage)
{
    switch (_stage)
    {
        case GL_VERTEX_SHADER:
            return ".cached_vulkan.vert";
        case GL_FRAGMENT_SHADER:
            return ".cached_vulkan.frag";
        default:
            NAMICA_CORE_ASSERT(false);
            return "";
    }
}

// 返回对应shader生成缓存文件的后缀名 - OpenGL类型
static char const* glShaderStageCachedOpenGLFileExtension(uint32_t _stage)
{
    switch (_stage)
    {
        case GL_VERTEX_SHADER:
            return ".cached_opengl.vert";
        case GL_FRAGMENT_SHADER:
            return ".cached_opengl.frag";
        default:
            NAMICA_CORE_ASSERT(false);
            return "";
    }
}

// 根据opengl的shader类型, 转换为vulkan中的glsl的shader类型
static shaderc_shader_kind glShaderStageToShaderC(GLenum _stage)
{
    switch (_stage)
    {
        case GL_VERTEX_SHADER:
            return shaderc_glsl_vertex_shader;
        case GL_FRAGMENT_SHADER:
            return shaderc_glsl_fragment_shader;
        default:
            NAMICA_CORE_ASSERT(false);
            return (shaderc_shader_kind)0;
    }
}

// opengl的shader类型转换为字符串
static char const* glShaderStageToString(GLenum _stage)
{
    switch (_stage)
    {
        case GL_VERTEX_SHADER:
            return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER:
            return "GL_FRAGMENT_SHADER";
        default:
            NAMICA_CORE_ASSERT(false);
            return "";
    }
}

// 根据shader文件中具体字符串判断OpenGL中的shader类型
static GLenum shaderTypeFromString(std::string const& _type)
{
    if (_type == "vertex")
    {
        return GL_VERTEX_SHADER;
    }
    else if (_type == "fragment" || _type == "pixel")
    {
        return GL_FRAGMENT_SHADER;
    }

    NAMICA_CORE_ASSERT(false);
    return 0;
}

std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(std::string const& _source)
{
    std::unordered_map<GLenum, std::string> shader_sources;
    char const* type_token = "#type";
    size_t type_token_len = strlen(type_token);
    size_t pos = _source.find(type_token, 0);
    while (pos != std::string::npos)
    {
        // 将#type 后的类型提取出匹配一下
        size_t eol = _source.find_first_of("\r\n", pos);  // 找到下一行开始
        NAMICA_CORE_ASSERT(eol != std::string::npos, "shader文件格式未能符合预期");
        size_t begin = pos + type_token_len + 1;  // #type [开始]
        std::string type = _source.substr(begin, eol - begin);
        GLenum key = shaderTypeFromString(type);
        size_t nex_line_pos = _source.find_first_of("\r\n", eol);
        pos = _source.find(type_token, nex_line_pos);
        shader_sources[key] = _source.substr(
            nex_line_pos,
            pos - (nex_line_pos == std::string::npos ? _source.size() - 1 : nex_line_pos));
    }

    return shader_sources;
}

void OpenGLShader::reflect(GLenum stage, std::vector<uint32_t> const& _shaderData)
{
    spirv_cross::Compiler compiler(_shaderData);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    NAMICA_CORE_DEBUG(
        "OpenGLShader::reflect - {0} {1}", glShaderStageToString(stage), m_filePath.c_str());
    NAMICA_CORE_DEBUG("    {0} uniform buffers", resources.uniform_buffers.size());
    NAMICA_CORE_DEBUG("    {0} resources", resources.sampled_images.size());

    NAMICA_CORE_DEBUG("Uniform buffers:");
    for (auto const& resource : resources.uniform_buffers)
    {
        auto const& bufferType = compiler.get_type(resource.base_type_id);
        auto bufferSize = compiler.get_declared_struct_size(bufferType);
        auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        auto memberCount = bufferType.member_types.size();

        NAMICA_CORE_DEBUG("  {0}", resource.name);
        NAMICA_CORE_DEBUG("    Size = {0}", bufferSize);
        NAMICA_CORE_DEBUG("    Binding = {0}", binding);
        NAMICA_CORE_DEBUG("    Members = {0}", memberCount);
    }
}

void OpenGLShader::compileOrGetVulkanBinaries(
    std::unordered_map<GLenum, std::string> const& _shaderSources)
{
    // 准备工作
    // GLSL/HLSL 代码 编译成 SPIR-V 二进制
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    bool const optimize = true;
    if (optimize)
    {
        // Shaderc以 性能优先 优化 SPIR-V 代码
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    std::filesystem::path cacheDirectory = getCacheDirectory();

    auto& shaderData = m_vulkanSPIRV;
    shaderData.clear();
    for (auto const& [stage, source] : _shaderSources)
    {
        std::filesystem::path shaderFilePath = m_filePath;
        // 得到对应shader的缓存路径, 进行拼接当前shader文件的hash值
        std::filesystem::path cachedPath = cacheDirectory /
            (shaderFilePath.filename().string() + m_shaderFileHash +
             glShaderStageCachedVulkanFileExtension(stage));

        std::ifstream in{cachedPath, std::ios::in | std::ios::binary};
        // 缓存文件存在, 根据hash值进行判断当前目标shader文件是否发生变动
        if (in.is_open())
        {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            auto& data = shaderData[stage];
            data.resize(size / sizeof(uint32_t));
            in.read((char*)data.data(), size);  // 将缓存文件的数据读入std::vector<uint32_t>内
        }
        else
        {
            // 开始编译
            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
                source, glShaderStageToShaderC(stage), m_filePath.c_str(), options);
            if (module.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                NAMICA_CORE_ERROR(module.GetErrorMessage());
                NAMICA_CORE_ASSERT(false, "SpvCompilationResult失败!");
            }

            shaderData[stage] = std::vector<uint32_t>{module.cbegin(), module.cend()};

            // bin 缓存文件保存
            std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
            if (out.is_open())
            {
                auto& data = shaderData[stage];
                out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                out.flush();
                out.close();
            }
        }
    }

    for (auto const& [stage, data] : shaderData)
    {
        // 反射, 将编译结果反应出来
        reflect(stage, data);
    }
}

void OpenGLShader::compileOrGetOpenGLBinaries()
{
    auto& shaderData = m_openglSPIRV;

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    bool const optimize = false;
    if (optimize)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    std::filesystem::path cache_directory = getCacheDirectory();

    shaderData.clear();
    m_openglSourceCode.clear();

    for (auto const& [stage, spirv] : m_vulkanSPIRV)
    {
        std::filesystem::path shader_file_path = m_filePath;
        std::filesystem::path cachedPath = cache_directory /
            (shader_file_path.filename().string() + m_shaderFileHash +
             glShaderStageCachedOpenGLFileExtension(stage));

        // 想找到opengl bin的缓存文件
        std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
        if (in.is_open())
        {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            auto& data = shaderData[stage];
            data.resize(size / sizeof(uint32_t));
            in.read((char*)data.data(), size);
        }
        else
        {
            // 否则, 就进行交叉编译
            // 先翻译为glsl code
            spirv_cross::CompilerGLSL glslCompiler(spirv);
            m_openglSourceCode[stage] = glslCompiler.compile();
            auto& source = m_openglSourceCode[stage];

            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
                source, glShaderStageToShaderC(stage), m_filePath.c_str());
            if (module.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                NAMICA_CORE_ERROR(module.GetErrorMessage());
                NAMICA_CORE_ASSERT(false, "SpvCompilationResult失败!");
            }

            shaderData[stage] = std::vector<uint32_t>{module.cbegin(), module.cend()};

            // opengl bin 缓存文件
            std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
            if (out.is_open())
            {
                auto& data = shaderData[stage];
                out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                out.flush();
                out.close();
            }
        }
    }
}

void OpenGLShader::createProgram()
{
    GLuint program = glCreateProgram();

    std::vector<GLuint> shaderIDs;
    for (auto&& [stage, spirv] : m_openglSPIRV)
    {
        GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));  // 先创建对应类型的shader
        glShaderBinary(1,
                       &shaderID,
                       GL_SHADER_BINARY_FORMAT_SPIR_V,
                       spirv.data(),
                       (uint32_t)(spirv.size() * sizeof(uint32_t)));  // 加载shader二进制内容
        glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);    // main类型?
        glAttachShader(program, shaderID);                            // 附加到shader程序上
    }

    glLinkProgram(program);  // 链接shader程序

    // 检查shader程序状态
    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        for (auto id : shaderIDs)
        {
            glDeleteShader(id);
        }

        NAMICA_CORE_ERROR("{}", infoLog.data());
        NAMICA_CORE_ASSERT(false, "shader 程序链接报错!");
        return;
    }

    // Always detach shaders after a successful link.
    for (auto id : shaderIDs)
    {
        glDetachShader(program, id);
        glDeleteShader(id);
    }

    m_rendererID = program;
}

OpenGLShader::OpenGLShader(std::string_view _filePath) : m_filePath(_filePath)
{
    // shader的缓存目录文件夹准备好
    createCacheDirectoryIfNeeded();

    auto [source, fileHash] = FileSystem::readFileAndHash(m_filePath);
    m_shaderFileHash = std::format("_{0}", fileHash);
    auto shaderSources = preProcess(source);

    {
        // 耗时统计
        Timer timer;
        // 1. vulkan->spir-v
        compileOrGetVulkanBinaries(shaderSources);
        // 2. vulkan bin -> opengl bin
        compileOrGetOpenGLBinaries();
        // 3. create shader program
        createProgram();
        NAMICA_CORE_WARN("Shader path:{0} 编译链接耗时: {1} ms", _filePath, timer.elapsedMillis());
    }

    // // 编译成功后, 取出文件路径中的文件名
    // auto last_slash = _filePath.find_last_of("/\\");  // 匹配最后一个`/`或者`\`
    // last_slash =
    //     last_slash == std::string::npos ? 0 : last_slash + 1;  // 找到/texture.shader的t下标位置
    // auto last_dot = _filePath.rfind('.');                      // 找到文件扩展名的位置
    // auto count = last_dot == std::string::npos ? _filePath.size() - last_slash
    //                                            : last_dot - last_slash;  // 计算文件名长度
    // m_name = _filePath.substr(last_slash, count);
}

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_rendererID);
}

uint32_t OpenGLShader::s_currentProgram{0};

void OpenGLShader::useProgram()
{
    if (s_currentProgram != m_rendererID)
    {
        s_currentProgram = m_rendererID;
        glUseProgram(s_currentProgram);
    }
}

void OpenGLShader::setMat4(std::string const& _name, glm::mat4 const& _value)
{
    useProgram();
    auto uniformLocation = glGetUniformLocation(m_rendererID, _name.c_str());
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(_value));
}

void OpenGLShader::setFloat4(std::string const& _name, glm::vec4 const& _value)
{
    useProgram();
    auto uniformLocation = glGetUniformLocation(m_rendererID, _name.c_str());
    glUniform4f(uniformLocation, _value.r, _value.g, _value.b, _value.a);
}

void OpenGLShader::setInt(std::string const& _name, int _value)
{
    useProgram();
    auto uniformLocation = glGetUniformLocation(m_rendererID, _name.c_str());
    glUniform1i(uniformLocation, _value);
}

void OpenGLShader::setIntArray(std::string const& _name, int* _value, uint32_t _count)
{
    useProgram();
    auto uniformLocation = glGetUniformLocation(m_rendererID, _name.c_str());
    glUniform1iv(uniformLocation, _count, _value);
}

void OpenGLShader::setFloat(std::string const& _name, float _value)
{
    useProgram();
    auto uniformLocation = glGetUniformLocation(m_rendererID, _name.c_str());
    glUniform1f(uniformLocation, _value);
}

}  // namespace Namica