#include "namica/core/Application.h"
#include <iostream>

namespace namica
{

Application::Application(ApplicationConfig const& appConfig) noexcept
{
}

Application::~Application()
{
}

void Application::run()
{
    // TODO: 文件结构测试
    std::cout << "你好, 朋友: 这里是Namica, 欢迎您的使用~" << std::endl;
}

}  // namespace namica