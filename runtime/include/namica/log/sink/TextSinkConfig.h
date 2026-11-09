#pragma once

#include "namica/log/sink/LogSinkConfig.h"
#include "namica/core/TimePoint.h"

namespace namica
{

#define NAMICA_DEFAULT_LOG_FMT "[{0}][{1}]{2}: {3}"

struct TextSinkConfig : LogSinkConfig
{
    String logFmt{NAMICA_DEFAULT_LOG_FMT};
    String dateFmt{NAMICA_TIME_HMS};

    TextSinkConfig() = default;
    TextSinkConfig(LogLevel _minLevel, String const& _logFmt, String const& _dateFmt)
        : LogSinkConfig{_minLevel}, logFmt{_logFmt}, dateFmt{_dateFmt}
    {
    }
};

}  // namespace namica