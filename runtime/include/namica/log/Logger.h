#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"
#include "namica/log/LogLevel.h"
#include "namica/log/LogRecord.h"

#include <vector>

namespace namica
{

class LogSink;

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
     * @brief 日志输出
     *
     * @tparam Args 格式化输出需要的对应值(可选)
     * @param _logLevel 日志级别
     * @param _msgfmt 格式化文本消息
     * @param _args 格式化输出需要的各个参数(可选)
     */
    template <typename... Args>
    void log(LogLevel _logLevel, String const& _msgFmt, Args&&... _args);

    [[nodiscard]] NAMICA_API Bool addSink(SharedPtr<LogSink> _sink);
    [[nodiscard]] NAMICA_API Bool removeSink(SharedPtr<LogSink> _sink);

    NAMICA_API void flush();

private:
    Logger();

    NAMICA_API void println(LogLevel _logLevel, String const& _msg);

private:
    std::vector<LogRecord> m_logRecords;
    std::vector<SharedPtr<LogSink>> m_logSinks;
};

}  // namespace namica

#include "namica/log/Logger.inl"