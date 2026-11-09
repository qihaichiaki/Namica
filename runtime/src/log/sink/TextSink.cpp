#include "namica/log/sink/TextSink.h"
#include "namica/log/sink/TextSinkConfig.h"
#include "namica/log/LogRecord.h"

#include <format>
#include <chrono>
#include <exception>

namespace namica
{

namespace
{

void validDateFmt(String const& _dateFormat)
{
    auto localTime{std::chrono::floor<std::chrono::seconds>(
        std::chrono::current_zone()->to_local(std::chrono::system_clock::now()))};
    static_cast<void>(std::vformat(_dateFormat, std::make_format_args(localTime)));
}

Bool tryIsValidDateFmt(String const& _dateFormat) noexcept
{
    try
    {
        validDateFmt(_dateFormat);
        return True;
    }
    catch (std::exception const&)
    {
        return False;
    }
}

void validLogFmt(String const& _logFmt)
{
    // 参数数量、顺序和类型必须与实际输出一致。
    String const time{"00:00:00"};
    String const level{"Info"};
    String const source{"source"};
    String const message{"message"};
    static_cast<void>(std::vformat(_logFmt, std::make_format_args(time, level, source, message)));
}

Bool tryIsValidLogFmt(String const& _logFmt) noexcept
{
    try
    {
        validLogFmt(_logFmt);
        return True;
    }
    catch (std::exception const&)
    {
        return False;
    }
}

}  // namespace

TextSink::TextSink(TextSinkConfig const& _config)
    : LogSink{_config}, m_logFmt{_config.logFmt}, m_dateFmt{_config.dateFmt}
{
    validDateFmt(this->m_dateFmt);
    validLogFmt(this->m_logFmt);
}

TextSink::~TextSink()
{
}

Bool TextSink::setLogFmt(String const& _logFmt)
{
    if (tryIsValidLogFmt(_logFmt))
    {
        this->m_logFmt = _logFmt;
        return True;
    }

    return False;
}

Bool TextSink::setDateFmt(String const& _dateFmt)
{
    if (tryIsValidDateFmt(_dateFmt))
    {
        this->m_dateFmt = _dateFmt;
        return True;
    }

    return False;
}

String TextSink::formatRecord(LogRecord const& _record) const
{
    String const timeStr{_record.timePoint.getStr(this->m_dateFmt)};
    String const& levelStr{getLogLevelStr(_record.logLevel)};

    return std::vformat(this->m_logFmt,
                        std::make_format_args(timeStr, levelStr, _record.source, _record.message));
}

}  // namespace namica