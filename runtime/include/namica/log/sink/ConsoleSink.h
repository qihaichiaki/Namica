#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"
#include "namica/log/sink/LogSink.h"

namespace namica
{

struct ConsoleSinkConfig;

class ConsoleSink final : public LogSink
{
public:
    NAMICA_API static SharedPtr<ConsoleSink> create(ConsoleSinkConfig const& _config);

public:
    /**
     * @brief 设置ConsoleSink的格式化输出
     *
     * @param _logFmt 日志输出格式(使用fmt格式, 其中0表示h:m:s的输出位置, 1表示level级别的输出位置,
     * 2表示格式化文本msg的输出位置)
     * @note 默认输出是: `[h:m:s][level] msg`
     *
     * @return 设置格式是否符合规范
     */
    NAMICA_API Bool setLogFmt(String const& _logFmt);

    NAMICA_API void write(LogRecord const& _record) override;
    NAMICA_API void flush() override;

private:
    ConsoleSink(ConsoleSinkConfig const& _config);

private:
    UInt8 m_logColorIndex[3];
};

}  // namespace namica