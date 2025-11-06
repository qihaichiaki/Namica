#pragma once

#include <namica/EntryPoint.h>
#include <namica/core/Application.h>

namespace namica
{

class NamicaEditor final : public Application
{
public:
    NamicaEditor(ApplicationConfig const& appConfig) : Application(appConfig)
    {
    }
};

Application* createApplication()
{
    ApplicationConfig appConfig{};
    appConfig.windowConfig.windowName = "NamicaEditor";
    // appConfig.workingDir = "";
    return new NamicaEditor{appConfig};
}

}  // namespace namica