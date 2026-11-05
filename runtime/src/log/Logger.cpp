#include "namica/log/Logger.h"

namespace namica
{

#define NAMICA_DEFAULT_LOG_FMT "[{0}][{1}] {2}"

Logger::Logger()
{
}

Logger::~Logger()
{
}

SharedPtr<Logger> Logger::create()
{
    return SharedPtr<Logger>{new Logger{}};
}

// static Bool isValidLogFmt(String const& _logFmt)
// {
//     // 参数数量、顺序和类型必须与实际输出一致。
//     String const time{"00:00:00"};
//     String const level{"Info"};
//     String const message{"message"};

//     try
//     {
//         // 只验证格式，不向控制台输出。
//         static_cast<void>(std::vformat(_logFmt, std::make_format_args(time, level, message)));
//         return true;
//     }
//     catch (std::format_error const&)
//     {
//         return false;
//     }
// }

// Bool Logger::setLogFmt(String const& _logFmt)
// {
//     if (isValidLogFmt(_logFmt))
//     {
//         this->m_logFmt = _logFmt;
//         return True;
//     }

//     return False;
// }

void Logger::println(LogLevel _logLevel, String const& _msg)
{
    // Int const levelIndex{static_cast<Int>(_logLevel)};
    // String const timeStr{getCurrentTime()};
    // String const& levelStr{levelStrs[levelIndex]};

    // std::cout << levelPreColor[levelIndex]
    //           << std::vformat(this->m_logFmt, std::make_format_args(timeStr, levelStr, _msg))
    //           << levelPostColor[levelIndex] << std::endl;
}

Bool Logger::addSink(SharedPtr<LogSink> _sink)
{
    if (_sink == nullptr)
    {
        return False;
    }

    for (auto const& sink : this->m_logSinks)
    {
        if (sink == _sink)
        {
            // 拒绝重复添加
            return False;
        }
    }

    this->m_logSinks.emplace_back(_sink);

    return True;
}

Bool Logger::removeSink(SharedPtr<LogSink> _sink)
{
    auto it{this->m_logSinks.begin()};
    while (it != this->m_logSinks.end())
    {
        if (*it == _sink)
        {
            this->m_logSinks.erase(it);
            return True;
        }
        it++;
    }

    return False;
}

void Logger::flush()
{
}

}  // namespace namica