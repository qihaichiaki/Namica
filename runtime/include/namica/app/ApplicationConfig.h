#pragma once

#include "namica/core/Types.h"
#include "namica/core/LogConfig.h"

namespace namica
{

struct ApplicationConfig
{
    String name{"Namica"};
    LogConfig logConfig{};
};

}  // namespace namica