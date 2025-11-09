#pragma once

#include <namica/EntryPoint.h>
#include <namica/core/ApplicationConfig.h>
#include <namica/core/Application.h>
namespace Namica
{

class NamicaEditor final : public Application
{
public:
    NamicaEditor(ApplicationConfig const& _appConfig) : Application(_appConfig)
    {
    }
};

Scope<Application> createApplication()
{
    // ApplicationConfig
    ApplicationConfig appConfig{};
    appConfig.windowConfig.windowName = "NamicaEditor";

    return createScope<NamicaEditor>(appConfig);
}

}  // namespace Namica