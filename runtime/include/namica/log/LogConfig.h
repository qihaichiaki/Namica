#pragma once

#include "namica/core/Types.h"
#include "namica/log/LogLevel.h"

namespace namica
{

struct LoggerConfig
{
    String logFmt{"[{0}][{1}] {2}"};
    LogLevel minLevel{LogLevel::Trace};
};

struct LogConfig
{
    LoggerConfig coreConfig{"[{0}][{1}]Core: {2}", LogLevel::Trace};
    LoggerConfig appConfig{"[{0}][{1}]App: {2}", LogLevel::Trace};
};

}  // namespace namica