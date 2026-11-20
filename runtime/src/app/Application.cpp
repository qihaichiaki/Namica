#include "namica/app/Application.h"
#include "namica/app/ApplicationConfig.h"
#include "namica/log/Log.h"
#include "namica/core/Exception.h"

namespace namica
{

static Application* s_instance{nullptr};

Application::Application(ApplicationConfig const& _config)
{
    if (s_instance)
    {
        throw Exception{"当前进程已有Application实例对象"};
    }

    // 进程Application唯一使用log尝试初始化
    Log::init(_config.logConfig);
    NAMICA_CORE_TRACE("Log初始化完成");

    s_instance = this;
}

Application::~Application()
{
    NAMICA_CORE_DEBUG("再见啦~");

    Log::shutdown();

    s_instance = nullptr;
}

Application* Application::get() noexcept
{
    return s_instance;
}

void Application::run()
{
    NAMICA_CORE_DEBUG("Namica正在运行哦");
}

}  // namespace namica