#pragma once

#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include "namica/core/Memory.h"
#include "namica/core/LayerStack.h"

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

    /**
     * @brief 推入层
     *
     * @param _layer 层级对象
     *
     * @note 推入越晚(越靠后的)层级事件接收优先级最高
     */
    NAMICA_API void pushLayer(Layer* _layer);

    /**
     * @brief 推入覆盖层
     *
     * @param _overlay 层级对象
     *
     * @note 推入越晚(越靠后的)层级事件接收优先级最高, 覆盖层当前推入就一定是最后一层
     */
    NAMICA_API void pushOverlay(Layer* _overlay);

    /**
     * @brief 弹出层
     *
     * @param _layer 层级对象
     */
    NAMICA_API void popLayer(Layer* _layer);

private:
    void onEvent(Event& _event);
    bool onWindowClose(WindowCloseEvent& _event);

private:
    Ref<Window> m_mainWindow{nullptr};
    bool m_isRunning{true};

    LayerStack m_layerStack;
};

// 声明创建application的函数, 必须在别处实现
Scope<Application> createApplication();

}  // namespace Namica
