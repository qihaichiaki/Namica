#pragma once

#include "namica_editor/EditorLayer.h"

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
        m_editorLayer = new EditorLayer{};
        pushLayer(m_editorLayer);
    }

    ~NamicaEditor()
    {
        popLayer(m_editorLayer);
        delete m_editorLayer;
        m_editorLayer = nullptr;
    }

private:
    Layer* m_editorLayer{nullptr};
};

Scope<Application> createApplication()
{
    // ApplicationConfig
    ApplicationConfig appConfig{};
    appConfig.windowConfig.title = "NamicaEditor";
    // appConfig.windowConfig.fullscreen = true;
    appConfig.rendererConfig.rendererAPIType = RendererAPIType::OpenGL;

    return createScope<NamicaEditor>(appConfig);
}

}  // namespace Namica