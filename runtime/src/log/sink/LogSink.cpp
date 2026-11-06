#include "namica/log/sink/LogSink.h"
#include "namica/log/sink/LogSinkConfig.h"

namespace namica
{

LogSink::LogSink(LogSinkConfig const& _config) : m_minLogLevel{_config.minLevel}
{
}

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