#pragma once

#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include "namica/core/Memory.h"

namespace Namica
{

struct ApplicationConfig;
class Window;
class Event;
class WindowCloseEvent;

class Application
{
public:
    CLASS_DISABLE_COPY_MOVE(Application)

    NAMICA_API Application(ApplicationConfig const& _appConfig) noexcept;
    NAMICA_API virtual ~Application();

    NAMICA_API static Application& get();

    NAMICA_API void run();

    /**
     * @brief 应用主动程序关闭循环
     */
    NAMICA_API void close();

private:
    void onEvent(Event& _event);
    bool onWindowClose(WindowCloseEvent& _event);

private:
    Ref<Window> m_mainWindow{nullptr};
    bool m_isRunning{true};
};

// 声明创建application的函数, 必须在别处实现
Scope<Application> createApplication();

}  // namespace Namica
