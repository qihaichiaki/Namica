#pragma once

#include "namica/core/Base.h"
#include "namica/core/WindowConfig.h"

namespace Namica
{

/** application相关配置 */
struct NAMICA_API ApplicationConfig
{
    WindowConfig windowConfig{};
    char const* workingDir{""};
};

}  // namespace Namica