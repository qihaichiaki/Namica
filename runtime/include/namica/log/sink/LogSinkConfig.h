#pragma once

#include "namica/log/LogLevel.h"

namespace namica
{

struct LogSinkConfig
{
    LogLevel minLevel{LogLevel::Trace};
};

}  // namespace namica