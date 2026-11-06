#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"
#include "namica/log/sink/LogSink.h"
#include "namica/log/sink/ConsoleSinkConfig.h"

namespace namica
{

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
    NAMICA_API Bool setDateFmt(String const& _dateFmt);

    NAMICA_API void setColorMode(ConsoleColorMode _colorMode);
    NAMICA_API void setColorTheme(ConsoleColorTheme const& _colorTheme);

    NAMICA_API Bool write(LogRecord const& _record) override;
    NAMICA_API Bool flush() override;

private:
    ConsoleSink(ConsoleSinkConfig const& _config);

private:
    std::ostream* m_os;
    String m_logFmt;
    String m_dateFmt;
    ConsoleColorMode m_colorMode;
    Bool m_colorEnabled;
    UInt8 m_logColorIndex[6];
};

}  // namespace namica