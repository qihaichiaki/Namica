#include "platform/glfw/GlfwWindow.h"
#include "platform/opengl/OpenGLContext.h"

namespace Namica
{

// TODO: 后续曾选择窗口的空间
Ref<Window> Window::create(WindowConfig const& _windowConfig)
{
    return createRef<GlfwWindow>(_windowConfig);
}

GlfwWindow::GlfwWindow(WindowConfig const& _windowConfig) : m_windowConfig{_windowConfig}
{
    init();
}

GlfwWindow::~GlfwWindow()
{
    shutdown();
}

void GlfwWindow::init()
{
    // make window

    // m_rendererContext = createScope<OpenGLContext>();
    // m_rendererContext->init();
}

void GlfwWindow::shutdown()
{
}

unsigned int GlfwWindow::getWidth() const noexcept
{
    return m_windowConfig.width;
}

unsigned int GlfwWindow::getHeight() const noexcept
{
    return m_windowConfig.height;
}

void GlfwWindow::setVSync(bool _enable)
{
}
bool GlfwWindow::getVSync() const noexcept
{
    return m_windowConfig.vsync;
}

}  // namespace Namica