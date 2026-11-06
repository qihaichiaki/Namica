#pragma once

#include "namica/core/Base.h"
#include "namica/log/LogLevel.h"

namespace namica
{

struct LogSinkConfig;

struct LogRecord;

class LogSink
{
public:
    NAMICA_API virtual ~LogSink() noexcept;

public:
    NAMICA_API void setLogMinLevel(LogLevel _level) noexcept;
    [[nodiscard]] NAMICA_API LogLevel getLogMinLevel() const noexcept;

    NAMICA_API virtual Bool write(LogRecord const& _record) = 0;
    NAMICA_API virtual Bool flush() = 0;

protected:
    NAMICA_API LogSink(LogSinkConfig const& _config);

protected:
    LogLevel m_minLogLevel;
};

}  // namespace namica