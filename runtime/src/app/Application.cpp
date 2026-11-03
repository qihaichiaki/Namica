#include "namica/app/Application.h"
#include "namica/core/Log.h"

#include <stdexcept>

namespace namica
{

static Application* s_instance{nullptr};

Application::Application(ApplicationConfig const& _config) : m_config{_config}
{
    if (s_instance)
    {
        throw std::logic_error{"Only one Application may exist at a time."};
    }

    // 进程Application唯一使用log尝试初始化
    Log::init();
    NAMICA_CORE_TRACE("Log初始化完成");

    s_instance = this;
}

Application::~Application()
{
    NAMICA_CORE_DEBUG("再见啦~");
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