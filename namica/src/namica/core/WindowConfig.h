#pragma once

#include "namica/core/Base.h"

namespace Namica
{

/** Window的相关配置 */
struct NAMICA_API WindowConfig
{
    char const* windowName{"namica"};
    unsigned int width{1600};
    unsigned int height{900};
    bool fullscreen{false};
    bool vsync{true};
    // char const* iconPath{};
};

}  // namespace Namica