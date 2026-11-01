#include "namica/core/Logger.h"
#include "namica/core/Time.h"
#include <iostream>

namespace namica
{

static String const levelStrs[]{"Trace", "Debug", "Info", "Warning", "Error", "Critical"};

Logger::Logger() : m_logFmt{NAMICA_LOG_FMT}, m_minLogLevel{LogLevel::Trace}
{
}

Logger::~Logger()
{
}

SharedPtr<Logger> Logger::create()
{
    return SharedPtr<Logger>{new Logger{}};
}

void Logger::setLogFmt(String const& _logFmt)
{
    this->m_logFmt = _logFmt;
}

void Logger::setLogMinLevel(LogLevel _logLevel)
{
    this->m_minLogLevel = _logLevel;
}

void Logger::println(LogLevel _logLevel, String const& _msg)
{
    String const timeStr{getCurrentTime()};
    String const& levelStr{levelStrs[static_cast<Int>(_logLevel)]};

    std::cout << std::vformat(this->m_logFmt, std::make_format_args(timeStr, levelStr, _msg))
              << std::endl;
}

}  // namespace namica