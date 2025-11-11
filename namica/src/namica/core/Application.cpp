#include "namica/core/Application.h"
#include "namica/core/ApplicationConfig.h"
#include "namica/core/Window.h"
#include "namica/core/Log.h"
#include "namica/events/WindowEvent.h"
#include "namica/utilities/FileSystem.h"

namespace Namica
{

static Application* s_instance{nullptr};

Application::Application(ApplicationConfig const& _appConfig) noexcept
{
    NAMICA_CORE_ASSERT(s_instance == nullptr);
    s_instance = this;

    FileSystem::setWorkingDirectory(_appConfig.workingDir);
    m_mainWindow = Window::create(_appConfig.windowConfig);
    m_mainWindow->setEventCallBackFn([this](Event& _event) { this->onEvent(_event); });
}

Application::~Application()
{
}

Application& Application::get()
{
    return *s_instance;
}

void Application::run()
{
    while (m_isRunning)
    {
        m_mainWindow->pollEvents();
        m_mainWindow->swapBuffers();
    }
}

void Application::close()
{
    m_isRunning = false;
}

void Application::onEvent(Event& _event)
{
    EventDispatcher dispatcher{_event};

    dispatcher.dispatch<WindowCloseEvent>(
        [this](WindowCloseEvent& _event) { return this->onWindowClose(_event); });

    // debug: 调试窗口信息
    NAMICA_CORE_DEBUG(_event.message());
}

bool Application::onWindowClose(WindowCloseEvent& _event)
{
    close();
    return false;  // 让其他层存在缓冲的机会
}

}  // namespace Namica