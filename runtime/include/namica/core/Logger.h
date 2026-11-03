#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"

namespace namica
{

enum class LogLevel
{
    Trace,     // 高频,非常细的执行过程
    Debug,     // 用于诊断的内部信息
    Info,      // 正常运行中的重要事件
    Warning,   // 出现异常，但仍能继续
    Error,     // 某项操作失败
    Critical,  // 严重故障,正常运行受到破坏
};

class Logger final
{
public:
    CLASS_DISABLE_COPY_MOVE(Logger)

    NAMICA_API ~Logger();

public:
    /**
     * @brief 创建默认格式化输出的的log
     */
    NAMICA_API static SharedPtr<Logger> create();

public:
    /**
     * @brief 设置日志的格式化输出
     *
     * @param _logFmt 日志输出格式(使用fmt格式, 其中0表示h:m:s的输出位置, 1表示level级别的输出位置,
     * 2表示格式化文本msg的输出位置)
     * @note 默认输出是: `[h:m:s][level] msg`
     *
     * @return 设置格式是否符合规范
     */
    NAMICA_API Bool setLogFmt(String const& _logFmt);

    /**
     * @brief 设置当前日志输出的最低日志级别(默认全输出)
     *
     * @param _logLevel 日志级别
     */
    NAMICA_API void setLogMinLevel(LogLevel _logLevel);

    /**
     * @brief 日志输出
     *
     * @tparam Args 格式化输出需要的对应值(可选)
     * @param _logLevel 日志级别
     * @param _msgfmt 格式化文本消息
     * @param _args 格式化输出需要的各个参数(可选)
     */
    template <typename... Args>
    void log(LogLevel _logLevel, String const& _msgFmt, Args&&... _args);

private:
    Logger();

    NAMICA_API void println(LogLevel _logLevel, String const& _msg);

private:
    String m_logFmt;
    LogLevel m_minLogLevel;
};

}  // namespace namica

#include "namica/core/Logger.inl"