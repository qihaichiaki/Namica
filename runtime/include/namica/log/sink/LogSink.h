#pragma once

#include "namica/core/Base.h"
#include "namica/log/LogLevel.h"

namespace namica
{

struct LogRecord;

class LogSink
{
public:
    NAMICA_API virtual ~LogSink() noexcept;

public:
    NAMICA_API void setLogMinLevel(LogLevel _level) noexcept;
    [[nodiscard]] NAMICA_API LogLevel getLogMinLevel() const noexcept;

    NAMICA_API virtual void write(LogRecord const& _record) = 0;
    NAMICA_API virtual void flush() = 0;

protected:
    LogLevel m_minLogLevel;
};

}  // namespace namica