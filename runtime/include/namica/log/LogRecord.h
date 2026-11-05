#pragma once

#include "namica/core/TimePoint.h"
#include "namica/log/LogLevel.h"

namespace namica
{

/**
 * @brief log日志记录
 */
struct LogRecord
{
    SystemTimePoint timePoint{};
    LogLevel logLevel{LogLevel::Info};
    String source{};
    String message{};
};

}  // namespace namica