#include "namica/core/Application.h"
#include "namica/core/Log.h"
namespace Namica
{

Application::Application(ApplicationConfig const& _appConfig) noexcept
{
}

Application::~Application()
{
}

void Application::run()
{
    // 日志测试
    NAMICA_CORE_DEBUG("debug内容测试");
    NAMICA_CORE_INFO("{}", "这是一跳正常的消息");
    NAMICA_CORE_WARN("注意, {0}内容出现警告: {1}", "Application", "run");
    NAMICA_CORE_ERROR("程序马上终止");

    NAMICA_APP_DEBUG("debug内容测试");
    NAMICA_APP_INFO("{}", "这是一跳正常的消息");
    NAMICA_APP_WARN("注意, {0}内容出现警告: {1}", "Application", "run");
    NAMICA_APP_ERROR("程序马上终止");
}

}  // namespace Namica