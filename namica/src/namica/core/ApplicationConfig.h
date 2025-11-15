#pragma once

#include "namica/core/Base.h"
#include "namica/core/WindowConfig.h"
#include "namica/renderer/RendererConfig.h"

namespace Namica
{

/** application相关配置 */
struct NAMICA_API ApplicationConfig
{
    char const* workingDir{""};
    WindowConfig windowConfig{};
    RendererConfig rendererConfig{};
};

}  // namespace Namica