#pragma once

#include "namica/log/sink/LogSink.h"
#include "namica/core/Types.h"

namespace namica
{

struct TextSinkConfig;

// 把一条LogRecord按日期和整行布局渲染成一行文本的Sink
class TextSink : public LogSink
{
public:
    NAMICA_API virtual ~TextSink();

public:
    /**
     * @brief 设置TextSink的格式化输出
     *
     * @param _logFmt 日志输出格式(使用fmt格式, 其中0表示h:m:s的输出位置, 1表示level级别的输出位置,
     * 2表示log的来源, 3表示格式化文本msg的输出位置)
     * @note 默认输出是: `[h:m:s][level]source msg`
     *
     * @return 设置格式是否符合规范
     */
    NAMICA_API Bool setLogFmt(String const& _logFmt);

    /**
     * @brief 设置日志输出时间的格式
     *
     * @param _dateFmt 格式化输出, %Y年 %m月 %d日 %H小时 %M分钟 %S秒, 需要包含{}, 内部:开头
     */
    NAMICA_API Bool setDateFmt(String const& _dateFmt);

    /**
     * @brief 输出当前日志记录的格式化后的文本行
     *
     * @param _logRecord 日志记录
     */
    [[nodiscard]] NAMICA_API String formatRecord(LogRecord const& _record) const;

protected:
    NAMICA_API TextSink(TextSinkConfig const& _config);

private:
    String m_logFmt;
    String m_dateFmt;
};

}  // namespace namica