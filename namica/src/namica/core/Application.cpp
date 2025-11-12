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
        // 窗口轮询时间
        m_mainWindow->pollEvents();

        // 层级更新
        for (auto layer : m_layerStack)
        {
            layer->onUpdate();
        }

        // 窗口交换缓冲区
        m_mainWindow->swapBuffers();
    }
}

void Application::close()
{
    m_isRunning = false;
}

void Application::pushLayer(Layer* _layer)
{
    NAMICA_CORE_ASSERT(_layer);
    m_layerStack.pushLayer(_layer);
    _layer->onAttach();
}

void Application::pushOverlay(Layer* _overlay)
{
    NAMICA_CORE_ASSERT(_overlay);
    m_layerStack.pushOverlay(_overlay);
    _overlay->onAttach();
}

void Application::popLayer(Layer* _layer)
{
    NAMICA_CORE_ASSERT(_layer);
    m_layerStack.popLayer(_layer);
    _layer->onDetach();
}

void Application::onEvent(Event& _event)
{
    // debug: 调试窗口信息
    NAMICA_CORE_DEBUG(_event.message());

    EventDispatcher dispatcher{_event};
    dispatcher.dispatch<WindowCloseEvent>(
        [this](WindowCloseEvent& _event) { return this->onWindowClose(_event); });

    // 从后往前遍历层级, 传递事件
    for (auto layerIt{m_layerStack.end()}; layerIt != m_layerStack.begin();)
    {
        // 事件被处理不在往前layer层传递
        if (_event.isHandled())
        {
            break;
        }
        (*(--layerIt))->onEvent(_event);
    }
}

bool Application::onWindowClose(WindowCloseEvent& _event)
{
    close();
    return false;  // 让其他层存在缓冲的机会
}

}  // namespace Namica