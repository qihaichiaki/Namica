#include "namica/log/sink/LogSink.h"

namespace namica
{

LogSink::~LogSink() noexcept
{
}

void LogSink::setLogMinLevel(LogLevel _level) noexcept
{
    this->m_minLogLevel = _level;
}

LogLevel LogSink::getLogMinLevel() const noexcept
{
    return this->m_minLogLevel;
}

}  // namespace namica