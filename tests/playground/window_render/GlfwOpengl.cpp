#include "playground/window_render/GlfwOpengl.h"

#include <iostream>

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
    Veci2 windowSize{static_cast<int>(mainWindowSize.first * _widthRatio),
                     static_cast<int>(mainWindowSize.second * _heightRatio)};
    return createWindow(_title, windowSize.x, windowSize.y);
}

void setWindowResizeEnable(GLFWwindow* const _window, bool _enable)
{
    glfwSetWindowAttrib(_window, GLFW_RESIZABLE, _enable ? GLFW_TRUE : GLFW_FALSE);
}

void destroyWindow(GLFWwindow* _window)
{
    glfwDestroyWindow(_window);
}

bool renderContextInit(GLFWwindow* _window)
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
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

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

GLuint createShaderProgram(std::string const& _vertexShaderSrc,
                           std::string const& _fragmentShaderSrc)
{
    GLuint shaderProgram{};

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

        return 0;
    }
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompileSuccess);
    if (isCompileSuccess == GL_FALSE)
    {
        char buffer[512]{};
        glGetShaderInfoLog(fragmentShader, 512, nullptr, buffer);
        std::cerr << "shader编译失败: " << buffer << std::endl;

        return 0;
    }

    // shaderProgram
    shaderProgram = glCreateProgram();
    // attach
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // link
    glLinkProgram(shaderProgram);
    GLint isLinkSuccess{};
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinkSuccess);
    if (isLinkSuccess == GL_FALSE)
    {
        char buffer[512]{};
        glGetProgramInfoLog(shaderProgram, 512, nullptr, buffer);
        std::cerr << "shaderProgram链接失败: " << buffer << std::endl;

        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}