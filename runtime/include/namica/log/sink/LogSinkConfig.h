#pragma once

#include "namica/log/LogLevel.h"
#include "namica/core/TimePoint.h"

namespace namica
{

struct LogSinkConfig
{
    LogLevel minLevel{LogLevel::Trace};
    String dateFormat{NAMICA_TIME_HMS};
};

}  // namespace namica