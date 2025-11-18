#pragma once

#include "namica/core/Window.h"
#include "namica/core/WindowConfig.h"

struct GLFWwindow;
namespace Namica
{

class RendererContext;
class GlfwWindow final : public Window
{
public:
    GlfwWindow(WindowConfig const& _windowConfig, RendererAPIType _rendererAPIType);
    ~GlfwWindow();

    virtual void setEventCallBackFn(EventCallBackFn _eventCallBackFn) override;
    virtual uint32_t getWidth() const noexcept override;
    virtual uint32_t getHeight() const noexcept override;
    virtual void setVSync(bool _enable) override;
    virtual bool getVSync() const noexcept override;
    virtual void pollEvents() override;
    virtual void swapBuffers() override;
    virtual RendererContext& getRendererContext() const override;

    /**
     * @brief 返回GLFW底层窗口句柄
     */
    GLFWwindow* getWindowHandle() const;

private:
    void init(RendererAPIType _rendererAPIType);
    void shutdown();

private:
    struct WindowData
    {
        char const* title{""};
        uint32_t width{0};
        uint32_t height{0};
        bool fullscreen{false};
        bool vsync{true};
        EventCallBackFn eventCallBackFn{nullptr};
    };

    WindowData m_windowData;
    GLFWwindow* m_window{nullptr};  // glfw窗口句柄
    Scope<RendererContext> m_rendererContext;
};

}  // namespace Namica