#include "namica/core/Logger.h"
#include "namica/core/Time.h"
#include <iostream>

namespace namica
{

// 字符映射
static String const levelStrs[]{"Trace", "Debug", "Info", "Warning", "Error", "Critical"};

// 颜色字符映射
// 默认 蓝绿 绿色 黄色 红色 红色
static char const* levelPreColor[]{"", "\033[36m", "\033[32m", "\033[33m", "\033[31m", "\033[31m"};
static char const* levelPostColor[]{"", "\033[0m", "\033[0m", "\033[0m", "\033[0m", "\033[0m"};

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

static Bool isValidLogFmt(String const& _logFmt)
{
    // 参数数量、顺序和类型必须与实际输出一致。
    String const time{"00:00:00"};
    String const level{"Info"};
    String const message{"message"};

    try
    {
        // 只验证格式，不向控制台输出。
        static_cast<void>(std::vformat(_logFmt, std::make_format_args(time, level, message)));
        return true;
    }
    catch (std::format_error const&)
    {
        return false;
    }
}

Bool Logger::setLogFmt(String const& _logFmt)
{
    if (isValidLogFmt(_logFmt))
    {
        this->m_logFmt = _logFmt;
        return True;
    }

    return False;
}

void Logger::setLogMinLevel(LogLevel _logLevel)
{
    this->m_minLogLevel = _logLevel;
}

void Logger::println(LogLevel _logLevel, String const& _msg)
{
    Int const levelIndex{static_cast<Int>(_logLevel)};
    String const timeStr{getCurrentTime()};
    String const& levelStr{levelStrs[levelIndex]};

    std::cout << levelPreColor[levelIndex]
              << std::vformat(this->m_logFmt, std::make_format_args(timeStr, levelStr, _msg))
              << levelPostColor[levelIndex] << std::endl;
}

}  // namespace namica