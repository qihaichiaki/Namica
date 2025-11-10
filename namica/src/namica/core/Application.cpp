#include "namica/core/Application.h"
#include "namica/core/ApplicationConfig.h"
#include "namica/core/Window.h"
#include "namica/core/Log.h"
#include "namica/utilities/FileSystem.h"

namespace Namica
{

static Application* s_instance{nullptr};

Application::Application(ApplicationConfig const& _appConfig) noexcept
{
    NAMICA_CORE_ASSERT(s_instance != nullptr);
    s_instance = this;

    FileSystem::setWorkingDirectory(_appConfig.workingDir);
    m_mainWindow = Window::create(_appConfig.windowConfig);
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
}

}  // namespace Namica