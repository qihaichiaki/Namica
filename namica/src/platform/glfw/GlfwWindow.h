#pragma once

#include "namica/core/Window.h"
#include "namica/core/WindowConfig.h"

namespace Namica
{

class RendererContext;
class GlfwWindow final : public Window
{
public:
    GlfwWindow(WindowConfig const& _windowConfig);
    ~GlfwWindow();

    virtual unsigned int getWidth() const noexcept override;
    virtual unsigned int getHeight() const noexcept override;
    virtual void setVSync(bool _enable) override;
    virtual bool getVSync() const noexcept override;

private:
    void init();
    void shutdown();

private:
    WindowConfig m_windowConfig;
    Scope<RendererContext> m_rendererContext;
};

}  // namespace Namica