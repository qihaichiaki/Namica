#include "platform/glfw/GlfwWindow.h"
#include "platform/opengl/OpenGLContext.h"
#include "namica/core/Log.h"
#include "namica/events/WindowEvent.h"

#include <GLFW/glfw3.h>

namespace Namica
{

// TODO: 后续曾选择窗口的空间
Ref<Window> Window::create(WindowConfig const& _windowConfig)
{
    return createRef<GlfwWindow>(_windowConfig);
}

void GlfwWindow::setEventCallBackFn(EventCallBackFn _eventCallBackFn)
{
    m_windowData.eventCallBackFn = _eventCallBackFn;
}

GlfwWindow::GlfwWindow(WindowConfig const& _windowConfig)
{
    m_windowData.title = _windowConfig.title;
    m_windowData.width = _windowConfig.width;
    m_windowData.height = _windowConfig.height;
    m_windowData.fullscreen = _windowConfig.fullscreen;
    m_windowData.vsync = _windowConfig.vsync;

    init();
}

GlfwWindow::~GlfwWindow()
{
    shutdown();
}

static bool s_isGlfwInit{false};

static void glfwErrorCallBack(int error, char const* description)
{
    NAMICA_CORE_ERROR("glfw error ({0}): {1}", error, description);
}

void GlfwWindow::init()
{
    // glfw初始化
    if (!s_isGlfwInit)
    {
        // 首先注册glfw错误处理回调, 方便已知glfw出错原因
        glfwSetErrorCallback(&glfwErrorCallBack);
        // glfw初始化内部系统资源
        NAMICA_CORE_ASSERT(glfwInit());
        s_isGlfwInit = true;
    }

    // glfw创建窗口
    if (m_windowData.fullscreen)
    {
        GLFWmonitor* monitor{glfwGetPrimaryMonitor()};
        GLFWvidmode const* mode{glfwGetVideoMode(monitor)};
        NAMICA_CORE_ASSERT(monitor && mode);
        m_window =
            glfwCreateWindow(mode->width, mode->height, m_windowData.title, monitor, nullptr);
    }
    else
    {
        m_window = glfwCreateWindow(
            m_windowData.width, m_windowData.height, m_windowData.title, nullptr, nullptr);
    }

    NAMICA_CORE_ASSERT(m_window);

    // 创建了渲染上下文, 渲染上下文进行初始化
    m_rendererContext = createScope<OpenGLContext>(m_window);
    m_rendererContext->init();

    // 设置是否垂直同步
    setVSync(m_windowData.vsync);

    // 设置用户数据, 方便在回调中取出使用
    glfwSetWindowUserPointer(m_window, &m_windowData);

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* _window) {
        WindowData& data{*(WindowData*)(glfwGetWindowUserPointer(_window))};

        WindowCloseEvent event{};
        data.eventCallBackFn(event);
    });

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* _window, int _width, int _height) {
        WindowData& data{*(WindowData*)(glfwGetWindowUserPointer(_window))};
        data.width = _width;
        data.height = _height;

        WindowResizeEvent event{_width, _height};
        data.eventCallBackFn(event);
    });

    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* _window, int _focused) {
        WindowData& data{*(WindowData*)(glfwGetWindowUserPointer(_window))};

        if (_focused == GLFW_TRUE)
        {
            WindowFocusEvent event{};
            data.eventCallBackFn(event);
        }
        else
        {
            WindowLostFocusEvent event{};
            data.eventCallBackFn(event);
        }
    });

    glfwSetWindowPosCallback(m_window, [](GLFWwindow* _window, int _xpos, int _ypos) {
        WindowData& data{*(WindowData*)(glfwGetWindowUserPointer(_window))};

        WindowMovedEvent event{_xpos, _ypos};
        data.eventCallBackFn(event);
    });
}

void GlfwWindow::shutdown()
{
    glfwDestroyWindow(m_window);
    m_window = nullptr;
}

unsigned int GlfwWindow::getWidth() const noexcept
{
    return m_windowData.width;
}

unsigned int GlfwWindow::getHeight() const noexcept
{
    return m_windowData.height;
}

void GlfwWindow::setVSync(bool _enable)
{
    m_windowData.vsync = _enable;
    glfwSwapInterval(static_cast<int>(_enable));
}

bool GlfwWindow::getVSync() const noexcept
{
    return m_windowData.vsync;
}

void GlfwWindow::pollEvents()
{
    glfwPollEvents();
}

void GlfwWindow::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

}  // namespace Namica