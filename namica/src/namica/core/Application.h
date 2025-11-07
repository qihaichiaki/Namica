#pragma once

#include "namica/core/ApplicationConfig.h"
#include "namica/core/Base.h"
#include "namica/core/TypeMacros.h"
#include "namica/core/Memory.h"

namespace Namica
{

class Application
{
public:
    CLASS_DISABLE_COPY_MOVE(Application)

    NAMICA_API Application(ApplicationConfig const& _appConfig) noexcept;
    NAMICA_API virtual ~Application();

    NAMICA_API void run();
};

// 声明创建application的函数, 必须在别处实现
Scope<Application> createApplication();

}  // namespace Namica
