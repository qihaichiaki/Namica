#include "playground/window_render/TestRender.h"

#include <iostream>
#include <namica/io/FileSystem.h>
#include <nlohmann/json.hpp>

namespace glfw_opengl
{

bool windowRenderInit()
{
    // 可在初始化前注册glfw错误回调函数, 函数类型为void(int error, const char* description)
    // glfwSetErrorCallback

    // glfwInit glfw初始化
    if (glfwInit() == GLFW_FALSE)
    {
        std::cerr << "glfw初始化失败!" << std::endl;
        return false;
    }
    else
    {
        std::cout << "glfw初始化成功!" << std::endl;
    }

    // glfw设置默认渲染opengl上下文的版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // 大版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  // 小版本号
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // profile
    // 使用core的理由: 只保留现代 OpenGL 推荐使用的功能，把很多旧式 OpenGL 功能去掉

    return true;
}

void windowShutdown()
{
    glfwTerminate();  // 终止glfw, 清理资源
}

std::pair<int, int> getPrimaryMonitorSize()
{
    // 获取主显示器
    GLFWmonitor* const monitor{glfwGetPrimaryMonitor()};
    // 获取主显示器相关模式
    GLFWvidmode const* monitorMode{glfwGetVideoMode(monitor)};
    return {monitorMode->width, monitorMode->height};
}

void setWindowCenterPos(GLFWwindow* const _window)
{
    auto const size{getPrimaryMonitorSize()};
    int const screenWidth{size.first};
    int const screenHeight{size.second};

    int windowWidth{};
    int windowHeight{};
    glfwGetWindowSize(_window, &windowWidth, &windowHeight);

    // 设置窗口位置
    // 设置到屏幕中间的位置
    glfwSetWindowPos(
        _window, screenWidth / 2 - windowWidth / 2, screenHeight / 2 - windowHeight / 2);
}

GLFWwindow* createWindow(std::string_view _title, GLFWwindow* _shareWindow, int _width, int _height)
{
    // share 共享opengl资源
    GLFWwindow* const glfwWindow{
        glfwCreateWindow(_width, _height, _title.data(), nullptr, _shareWindow)};
    if (glfwWindow == nullptr)
    {
        std::cerr << "glfw创建窗口失败!" << std::endl;
    }
    else
    {
        std::cout << "glfw窗口创建成功!" << std::endl;
        setWindowCenterPos(glfwWindow);
    }

    return glfwWindow;
}

GLFWwindow* createWindow(std::string_view _title, int _width, int _height)
{
    // 创建glfw窗口

    // 注意此处传入monitor是指在哪个显示器上创建全屏窗口
    GLFWwindow* const glfwWindow{
        glfwCreateWindow(_width, _height, _title.data(), nullptr, nullptr)};
    if (glfwWindow == nullptr)
    {
        std::cerr << "glfw创建窗口失败!" << std::endl;
    }
    else
    {
        std::cout << "glfw窗口创建成功!" << std::endl;
        setWindowCenterPos(glfwWindow);
    }

    return glfwWindow;
}

GLFWwindow* createWindow(std::string_view _title, float _widthRatio, float _heightRatio)
{
    auto mainWindowSize{getPrimaryMonitorSize()};
    namica::Veci2 windowSize{static_cast<int>(mainWindowSize.first * _widthRatio),
                             static_cast<int>(mainWindowSize.second * _heightRatio)};
    return createWindow(_title, windowSize.x(), windowSize.y());
}

std::pair<int, int> getWindowSize(GLFWwindow* const _window)
{
    std::pair<int, int> size{};
    glfwGetWindowSize(_window, &(size.first), &(size.second));
    return size;
}

void setWindowResizeEnable(GLFWwindow* const _window, bool _enable)
{
    glfwSetWindowAttrib(_window, GLFW_RESIZABLE, _enable ? GLFW_TRUE : GLFW_FALSE);
}

void destroyWindow(GLFWwindow* _window)
{
    glfwDestroyWindow(_window);
}

bool renderContextInit(GLFWwindow* _window, bool depthTest)
{
    // 创建完窗口后, glfw设置指定窗口为当前线程的opengl上下文
    // 多窗口的切换渲染也利用此进行实现, 但是gl的全局函数加载只需要一次即可
    glfwMakeContextCurrent(_window);

    // 设置完毕后通过获取opengl上下文函数地址, 初始化glad, 使glad加载现代opengl函数
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        std::cerr << "glad 加载OpenGL函数失败!" << std::endl;
        return false;
    }

    // 打印加载的opengl上下文信息
    std::cout << "OpenGL的实现方[vendor]:    " << glGetString(GL_VENDOR)
              << std::endl;  // opengl的实现方
    std::cout << "实际渲染的GPU[renderer]:   " << glGetString(GL_RENDERER)
              << std::endl;  // 用来实际渲染的GPU
    std::cout << "OpenGL的实际版本[version]: " << glGetString(GL_VERSION)
              << std::endl;  // opengl的实际版本

    // 启用纹理混合
    glEnable(GL_BLEND);
    // 经典混合函数, 实现透明效果
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (depthTest)
    {
        // 启用深度测试
        glEnable(GL_DEPTH_TEST);
    }

    return true;
}

bool windowShouldClose(GLFWwindow* const _window)
{
    return glfwWindowShouldClose(_window);
}

void swapBuffers(GLFWwindow* const _window)
{
    // 交换窗口的颜色缓冲区(双缓冲区, 防止屏幕出现残影等)
    glfwSwapBuffers(_window);
}

void pollEvents()
{
    // glfw利用下面的函数进行处理**窗口事件**
    glfwPollEvents();
}

}  // namespace glfw_opengl

// Texture
Texture::Texture(namica::Int const _width, namica::Int const _height, namica::UChar const* _data)
{
    glGenTextures(1, &m_textureObj);
    glBindTexture(GL_TEXTURE_2D, m_textureObj);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _data);

    glGenerateMipmap(GL_TEXTURE_2D);

    // 贴图环绕
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 贴图过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureObj);
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_textureObj);
}

std::shared_ptr<Texture> Texture::load(namica::FileSystem& _fileSystem,
                                       std::filesystem::path const& _texturePath)
{
    std::shared_ptr<Texture> texture{};

    namica::Int textureWidth{};
    namica::Int textureHeight{};
    namica::Int textureChannels{};
    auto textureBuffer{
        _fileSystem.loadAssetImage(_texturePath, textureWidth, textureHeight, textureChannels)};
    if (!textureBuffer.empty())
    {
        std::cout << "已成功加载图片: " << _texturePath << std::endl;
        std::cout << "宽度: " << textureWidth << std::endl;
        std::cout << "高度: " << textureHeight << std::endl;
        std::cout << "通道数: " << textureChannels << std::endl;

        texture = std::make_shared<Texture>(textureWidth, textureHeight, textureBuffer.data());
    }

    return texture;
}

// ShaderProgram
ShaderProgram::ShaderProgram(std::string const& _vertexShaderSrc,
                             std::string const& _fragmentShaderSrc)
{
    GLuint vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    GLuint fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};

    char const* vertexShaderSourceCStr{_vertexShaderSrc.c_str()};
    glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
    char const* fragmentShaderSourceCStr{_fragmentShaderSrc.c_str()};
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);

    // Compile
    glCompileShader(vertexShader);
    GLint isCompileSuccess{};
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompileSuccess);
    if (isCompileSuccess == GL_FALSE)
    {
        char buffer[512]{};
        glGetShaderInfoLog(vertexShader, 512, nullptr, buffer);
        std::cerr << "shader编译失败: " << buffer << std::endl;

        return;
    }
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompileSuccess);
    if (isCompileSuccess == GL_FALSE)
    {
        char buffer[512]{};
        glGetShaderInfoLog(fragmentShader, 512, nullptr, buffer);
        std::cerr << "shader编译失败: " << buffer << std::endl;

        return;
    }

    // shaderProgram
    m_shaderProgram = glCreateProgram();
    // attach
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    // link
    glLinkProgram(m_shaderProgram);
    GLint isLinkSuccess{};
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &isLinkSuccess);
    if (isLinkSuccess == GL_FALSE)
    {
        char buffer[512]{};
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, buffer);
        std::cerr << "shaderProgram链接失败: " << buffer << std::endl;

        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_shaderProgram);
}

void ShaderProgram::bind()
{
    // 使用当前的shader程序, 便于后续的uniform数据上传和渲染
    glUseProgram(m_shaderProgram);

    m_curTextureIndex = 0;  // 置空
}

GLint ShaderProgram::getUniformLocation(std::string const& _id)
{
    if (m_uniformLocation.find(_id) == m_uniformLocation.end())
    {
        m_uniformLocation[_id] = glGetUniformLocation(m_shaderProgram, _id.data());
    }

    return m_uniformLocation[_id];
}

void ShaderProgram::setParam(std::string const& _id, namica::Float const& _value)
{
    glUniform1f(this->getUniformLocation(_id), _value);
}

void ShaderProgram::setParam(std::string const& _id, namica::Vec2 const& _value)
{
    glUniform2f(this->getUniformLocation(_id), _value.x(), _value.y());
}

void ShaderProgram::setParam(std::string const& _id, namica::Vec3 const& _value)
{
    glUniform3f(this->getUniformLocation(_id), _value.x(), _value.y(), _value.z());
}

void ShaderProgram::setParam(std::string const& _id, namica::Vec4 const& _value)
{
    glUniform4f(this->getUniformLocation(_id), _value.x(), _value.y(), _value.z(), _value.w());
}

void ShaderProgram::setParam(std::string const& _id, namica::Mat4 const& _value)
{
    glUniformMatrix4fv(this->getUniformLocation(_id), 1, GL_FALSE, _value.data());
}

void ShaderProgram::setParam(std::string const& _id, Texture* _value)
{
    glActiveTexture(GL_TEXTURE0 + m_curTextureIndex);
    _value->bind();
    glUniform1i(this->getUniformLocation(_id), m_curTextureIndex);
    m_curTextureIndex++;
}

// Material
Material::Material(std::shared_ptr<ShaderProgram> const& _shaderProgram)
    : m_shaderProgram{_shaderProgram}
{
}

ShaderProgram& Material::getShaderProgram()
{
    return *m_shaderProgram;
}

void Material::setParam(std::string const& _id, namica::Float const& _value)
{
    m_floatData[_id] = _value;
}

void Material::setParam(std::string const& _id, namica::Vec2 const& _value)
{
    m_vec2Data[_id] = _value;
}

void Material::setParam(std::string const& _id, namica::Vec3 const& _value)
{
    m_vec3Data[_id] = _value;
}

void Material::setParam(std::string const& _id, namica::Vec4 const& _value)
{
    m_vec4Data[_id] = _value;
}

void Material::setParam(std::string const& _id, std::shared_ptr<Texture> const& _value)
{
    m_textureData[_id] = _value;
}

void Material::bind()
{
    m_shaderProgram->bind();

    // 遍历data, 依次设置值
    for (auto& [id, value] : m_floatData)
    {
        m_shaderProgram->setParam(id, value);
    }
    for (auto& [id, value] : m_vec2Data)
    {
        m_shaderProgram->setParam(id, value);
    }
    for (auto& [id, value] : m_vec3Data)
    {
        m_shaderProgram->setParam(id, value);
    }
    for (auto& [id, value] : m_vec4Data)
    {
        m_shaderProgram->setParam(id, value);
    }
    for (auto& [id, value] : m_textureData)
    {
        m_shaderProgram->setParam(id, value.get());
    }
}

std::shared_ptr<Material> Material::load(namica::FileSystem& _fileSystem,
                                         std::filesystem::path const& _materialPath)
{
    using namespace nlohmann;
    json const jsonRoot{json::parse(_fileSystem.loadAssetFileText(_materialPath))};

    std::shared_ptr<Material> material{nullptr};

    if (jsonRoot.contains("Shader"))
    {
        auto const shaderData{jsonRoot["Shader"]};
        if (shaderData.contains("Vertex") && shaderData.contains("Fragment"))
        {
            std::string const vertexShaderSRC{
                _fileSystem.loadAssetFileText(shaderData["Vertex"].get<std::string>())};
            std::string const fragmentShaderSRC{
                _fileSystem.loadAssetFileText(shaderData["Fragment"].get<std::string>())};
            material = std::make_shared<Material>(
                std::make_shared<ShaderProgram>(vertexShaderSRC, fragmentShaderSRC));

            if (jsonRoot.contains("Data"))
            {
                auto const materialData{jsonRoot["Data"]};

                if (materialData.contains("Float"))
                {
                    auto const floatData{materialData["Float"]};
                    for (auto const& item : floatData)
                    {
                        std::string const& id{item["name"].get<std::string>()};
                        namica::Float const value{item["value"].get<namica::Float>()};
                        material->setParam(id, value);
                    }
                }

                if (materialData.contains("Vec2"))
                {
                    auto const vec2Data{materialData["Vec2"]};
                    for (auto const& item : vec2Data)
                    {
                        std::string const& id{item["name"].get<std::string>()};
                        namica::Vec2 const value{item["value0"].get<namica::Float>(),
                                                 item["value1"].get<namica::Float>()};
                        material->setParam(id, value);
                    }
                }

                if (materialData.contains("Vec3"))
                {
                    auto const vec3Data{materialData["Vec3"]};
                    for (auto const& item : vec3Data)
                    {
                        std::string const& id{item["name"].get<std::string>()};
                        namica::Vec3 const value{item["value0"].get<namica::Float>(),
                                                 item["value1"].get<namica::Float>(),
                                                 item["value2"].get<namica::Float>()};
                        material->setParam(id, value);
                    }
                }

                if (materialData.contains("Vec4"))
                {
                    auto const vec4Data{materialData["Vec4"]};
                    for (auto const& item : vec4Data)
                    {
                        std::string const& id{item["name"].get<std::string>()};
                        namica::Vec4 const value{item["value0"].get<namica::Float>(),
                                                 item["value1"].get<namica::Float>(),
                                                 item["value2"].get<namica::Float>(),
                                                 item["value3"].get<namica::Float>()};
                        material->setParam(id, value);
                    }
                }

                if (materialData.contains("Texture"))
                {
                    auto const textureData{materialData["Texture"]};
                    for (auto const& item : textureData)
                    {
                        std::string const& id{item["name"].get<std::string>()};
                        std::filesystem::path const texturePath{item["path"].get<std::string>()};
                        material->setParam(id, Texture::load(_fileSystem, texturePath));
                    }
                }
            }
        }
    }

    return material;
}

// VertexElement
VertexElement::VertexElement(GLenum dataType, GLint _dataSize)
    : dataType{dataType}, dataSize{_dataSize}
{
    switch (dataType)
    {
        case GL_FLOAT:
            dataByte = sizeof(float) * dataSize;
            break;
        case GL_INT:
            dataByte = sizeof(int) * dataSize;
            break;
        default:
            break;
    }
}

// VertexLayout
VertexLayout::VertexLayout(std::initializer_list<VertexElement> const& _elements)
    : m_elements{_elements}, m_stride{0}
{
    namica::UInt index{0};
    for (auto& element : m_elements)
    {
        element.index = index++;
        element.offset = m_stride;
        m_stride += element.dataByte;
    }
}

void VertexLayout::push(VertexElement const& _element)
{
    VertexElement element{_element};
    element.index = (namica::UInt)m_elements.size();
    element.offset = m_stride;
    m_stride += element.dataByte;
    m_elements.push_back(element);
}

std::vector<VertexElement>::iterator VertexLayout::begin()
{
    return m_elements.begin();
}

std::vector<VertexElement>::iterator VertexLayout::end()
{
    return m_elements.end();
}

std::vector<VertexElement>::const_iterator VertexLayout::begin() const
{
    return m_elements.begin();
}

std::vector<VertexElement>::const_iterator VertexLayout::end() const
{
    return m_elements.end();
}

GLsizei VertexLayout::getStride() const
{
    return m_stride;
}

// Mesh
Mesh::Mesh(VertexLayout const& _vertexLayout,
           std::vector<namica::Float> const& _vertices,
           std::vector<namica::UInt> const& _indices)
    : m_vertexLayout{_vertexLayout}, m_indexCount{_indices.size()}
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(namica::Float) * _vertices.size(),
                 _vertices.data(),
                 GL_STATIC_DRAW);

    // 设置顶点布局
    for (auto const& vertexElement : m_vertexLayout)
    {
        glVertexAttribPointer(vertexElement.index,
                              vertexElement.dataSize,
                              vertexElement.dataType,
                              vertexElement.normalized,
                              m_vertexLayout.getStride(),
                              (void*)(uintptr_t)(vertexElement.offset));
        glEnableVertexAttribArray(vertexElement.index);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(namica::UInt) * _indices.size(),
                 _indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::draw()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
}

// Transform
namica::Mat4 Transform::getTransform() const
{
    namica::Mat4 model{1.0f};

    model.translate(position);
    // model.rotate(namica::radians(rotation.x()), namica::Vec3{1.0f, 0.0f, 0.0f});
    // model.rotate(namica::radians(rotation.y()), namica::Vec3{0.0f, 1.0f, 0.0f});
    // model.rotate(namica::radians(rotation.z()), namica::Vec3{0.0f, 0.0f, 1.0f});
    model *= rotation.toMatrix();
    model.scale(scale);

    return model;
}

// Camera
Transform& Camera::getTransform()
{
    return m_trasf;
}
CameraData& Camera::getData()
{
    return m_data;
}

namica::Mat4 Camera::getView() const
{
    // namica::Mat4 viewMat{cameraTransform.getTransform().inversed()};
    namica::Mat4 viewMat{m_trasf.rotation.toMatrix()};
    viewMat[3] = namica::Vec4{m_trasf.position, 1.0f};
    return viewMat.inverse();
}

namica::Mat4 Camera::getProject() const
{
    return namica::Mat4::perspective(m_data.fov, m_data.aspect, m_data.zNear, m_data.zFar);
}

// PlayerController
PlayerController::PlayerController(Camera& _camera) : m_camera(_camera)
{
}

void PlayerController::init(GLFWwindow* const _window)
{
    // 注册事件响应回调
    glfwSetWindowUserPointer(_window, this);

    // 键盘回调 void (* GLFWkeyfun)(GLFWwindow* window, int key, int scancode, int action, int
    // mods);
    glfwSetKeyCallback(_window,
                       [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods) {
                           PlayerController* const controller{
                               static_cast<PlayerController*>(glfwGetWindowUserPointer(_window))};
                           controller->onKeyEvent(_key, _scancode, _action, _mods);
                       });

    // 鼠标按键回调: void (* GLFWmousebuttonfun)(GLFWwindow* window, int button, int action, int
    // mods);
    glfwSetMouseButtonCallback(
        _window, [](GLFWwindow* _window, int _button, int _action, int _mods) {
            PlayerController* const controller{
                static_cast<PlayerController*>(glfwGetWindowUserPointer(_window))};
            controller->onMouseEvent(_button, _action, _mods);
        });

    // 鼠标移动位置回调：typedef void (* GLFWcursorposfun)(GLFWwindow* window, double xpos,
    // double ypos);
    glfwSetCursorPosCallback(_window, [](GLFWwindow* _window, double _xpos, double _ypos) {
        PlayerController* const controller{
            static_cast<PlayerController*>(glfwGetWindowUserPointer(_window))};
        controller->onMousePosEvent(
            namica::Vec2{static_cast<namica::Float>(_xpos), static_cast<namica::Float>(_ypos)});
    });
}

void PlayerController::onKeyEvent(int _key, int _scancode, int _action, int _mods)
{
    if (_key == GLFW_KEY_A)
    {
        m_moveKeyState[0] = _action != GLFW_RELEASE;
    }
    if (_key == GLFW_KEY_D)
    {
        m_moveKeyState[1] = _action != GLFW_RELEASE;
    }
    if (_key == GLFW_KEY_W)
    {
        m_moveKeyState[2] = _action != GLFW_RELEASE;
    }
    if (_key == GLFW_KEY_S)
    {
        m_moveKeyState[3] = _action != GLFW_RELEASE;
    }
}

void PlayerController::onMouseEvent(int _button, int _action, int _mods)
{
    if (_button == GLFW_MOUSE_BUTTON_LEFT)
    {
        m_mouseLeftState = _action != GLFW_RELEASE;
    }
}

void PlayerController::onMousePosEvent(namica::Vec2 const& _pos)
{
    m_mousePos = _pos;
}

void PlayerController::onUpdate(namica::Float const _deltaTime)
{
    Transform& cameraTransform{m_camera.getTransform()};

    // 计算旋转
    if (m_mouseLeftState)
    {
        namica::Float const deltaX{m_mousePos.x() - m_mousePosOld.x()};
        namica::Float const deltaY{m_mousePos.y() - m_mousePosOld.y()};

        // deltaX 围绕着y轴转, 注意这里给予的值是逆时针方向
        namica::Float const yAngle{-deltaX * m_sensitivity * _deltaTime};
        namica::Quat yRot{namica::Quat::angleAxis(yAngle, namica::Vec3{0.0f, 1.0f, 0.0f})};

        // deltaY 围绕着x轴转
        namica::Float const xAngle{-deltaY * m_sensitivity * _deltaTime};
        namica::Vec3 right{cameraTransform.rotation * namica::Vec3{1.0f, 0.0f, 0.0f}};
        namica::Quat xRot{namica::Quat::angleAxis(xAngle, right)};

        namica::Quat deltaRot{yRot * xRot};
        cameraTransform.rotation = (deltaRot * cameraTransform.rotation).normalized();
    }

    // 计算平移

    // 计算当前基于相机transform的right和front方向
    // 右手坐标系, 拇指为x正轴
    namica::Vec3 right{cameraTransform.rotation * namica::Vec3{1.0f, 0.0f, 0.0f}};
    // 同理, 中指为z正轴, 相机是朝向负z轴的
    namica::Vec3 front{cameraTransform.rotation * namica::Vec3{0.0f, 0.0f, -1.0f}};

    if (m_moveKeyState[0])  // A
    {
        cameraTransform.position -= right * m_moveSpeed * _deltaTime;
    }
    if (m_moveKeyState[1])  // D
    {
        cameraTransform.position += right * m_moveSpeed * _deltaTime;
    }
    if (m_moveKeyState[2])  // W
    {
        cameraTransform.position += front * m_moveSpeed * _deltaTime;
    }
    if (m_moveKeyState[3])  // S
    {
        cameraTransform.position -= front * m_moveSpeed * _deltaTime;
    }

    // 更新历史鼠标位置
    m_mousePosOld = m_mousePos;
}