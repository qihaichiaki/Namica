#include "namica/core/Logger.h"
#include "namica/core/Time.h"
#include <iostream>

namespace Namica
{

static std::string_view logLevelToStr(LogLevel _level)
{
    using enum LogLevel;
    switch (_level)
    {
        case Debug:
            return "DEBUG";
        case Info:
            return "INFO";
        case Warn:
            return "WARN";
        case Error:
            return "ERROR";
    }

    return "UNKNOWN";
}

static std::string colorize(LogLevel _level, std::string const& _msg)
{
    switch (_level)
    {
        case LogLevel::Debug:
            return "\033[36m" + _msg + "\033[0m";  // 蓝绿
        case LogLevel::Info:
            return "\033[32m" + _msg + "\033[0m";  // 绿色
        case LogLevel::Warn:
            return "\033[33m" + _msg + "\033[0m";  // 黄色
        case LogLevel::Error:
            return "\033[31m" + _msg + "\033[0m";  // 红色
    }

    return _msg;
}

Ref<Logger> Logger::create()
{
    return Ref<Logger>{new Logger{}};
}

Logger::Logger()
{
}

void Logger::setLogFmt(std::string_view _logFmt)
{
    m_logFmt = _logFmt;
}

void Logger::logOutput(LogLevel _logLevel, std::string_view _msg) const
{
    // TODO: cout
    auto timeStr = TimeUtils::localTime(TimeUtils::TimeFmtOption::HMS);
    auto levlStr = logLevelToStr(_logLevel);

    std::cout << colorize(_logLevel,
                          std::vformat(m_logFmt, std::make_format_args(timeStr, levlStr, _msg)))
              << std::endl;
}

}  // namespace Namica