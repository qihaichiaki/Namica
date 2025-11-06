#pragma once

#include "namica/core/Base.h"
#include "namica/core/WindowConfig.h"

namespace namica
{

/** application相关配置 */
struct NAMICA_API ApplicationConfig
{
    WindowConfig windowConfig{};
    char const* workingDir{nullptr};
};

}  // namespace namica