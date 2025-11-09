#include "namica/core/Application.h"
#include "namica/core/ApplicationConfig.h"
#include "namica/utilities/FileSystem.h"

namespace Namica
{

Application::Application(ApplicationConfig const& _appConfig) noexcept
{
    FileSystem::setWorkingDirectory(_appConfig.workingDir);
}

Application::~Application()
{
}

void Application::run()
{
}

}  // namespace Namica