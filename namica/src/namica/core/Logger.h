#pragma once

#include "namica/core/Base.h"
#include "namica/core/Memory.h"
#include <format>

namespace Namica
{

enum class LogLevel
{
    Debug,
    Info,
    Warn,
    Error
};

class Logger final
{
public:
    /**
     * @brief Set the Log Fmt object
     *
     * @param _logFmt 日志输出格式, 请使用fmt格式, 其中0表示h:m:s输出位置, 1表示level级别输出位置,
     * 2表示msg输出位置
     */
    NAMICA_API void setLogFmt(std::string_view _logFmt);

    /**
     * @brief 日志器输出
     *
     * @tparam Args fmt的参数包类型
     * @param _logLevel 日志输出级别
     * @param _fmt 日志格式化内容
     * @param _args 格式化内容的所需参数
     */
    template <typename... Args>
    void log(LogLevel _logLevel, std::string_view _fmt, Args&&... _args) const
    {
        // 注意, format适用于编译期间决定的, fmt这些并非变量, 所以需要使用vformat使其可以传入变量
        // auto msg = std::format(_fmt, std::forward<Args const&>(_args)...);
        auto msg = std::vformat(_fmt, std::make_format_args(std::forward<Args const&>(_args)...));
        logOutput(_logLevel, msg);
    }

    template <typename... Args>
    void log(LogLevel _logLevel,
             std::string_view _prefix,
             std::format_string<Args...> _fmt,
             Args&&... _args) const
    {
        auto formatted = std::format(_fmt, std::forward<Args>(_args)...);
        log(_logLevel, "{0}: {1}", _prefix, formatted);
    }

public:
    /**
     * @brief 创建Logger引用对象
     */
    NAMICA_API static Ref<Logger> create();

private:
    void logOutput(LogLevel _logLevel, std::string_view _msg) const;

private:
    Logger();
    Logger(Logger&) = delete;

private:
    std::string m_logFmt{"[{0}][{1}] {2}"};  // [h:m:s][level] msg

    FRIEND_REF_FUNC
};

}  // namespace Namica