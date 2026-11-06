#include "namica/log/sink/ConsoleSink.h"
#include "namica/core/Base.h"
#include "namica/log/LogRecord.h"

#include <iostream>
#include <format>

#ifdef NAMICA_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace namica
{

#ifdef NAMICA_PLATFORM_WINDOWS

Bool tryEnableAnsiColor(HANDLE _handle) noexcept
{
    if (_handle == nullptr || _handle == INVALID_HANDLE_VALUE)
    {
        return False;
    }

    DWORD mode{};
    if (GetConsoleMode(_handle, &mode) == 0)
    {
        return False;
    }

    DWORD const required{ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING};

    if ((mode & required) == required)
    {
        return True;
    }

    return SetConsoleMode(_handle, mode | required) != 0;
}

#else

#endif

// 颜色字符映射
// 默认 蓝绿 绿色 黄色 红色
static char const* levelPreColor[]{"", "\033[36m", "\033[32m", "\033[33m", "\033[31m"};
static char const* levelPostColor[]{"", "\033[0m", "\033[0m", "\033[0m", "\033[0m"};

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
    String const message{"message"};
    static_cast<void>(std::vformat(_logFmt, std::make_format_args(time, level, message)));
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

SharedPtr<ConsoleSink> ConsoleSink::create(ConsoleSinkConfig const& _config)
{
    return SharedPtr<ConsoleSink>{new ConsoleSink{_config}};
}

ConsoleSink::ConsoleSink(ConsoleSinkConfig const& _config)
    : LogSink{_config},
      m_os{_config.target == ConsoleTarget::Stdout ? &std::cout : &std::cerr},
      m_logFmt{_config.logFmt},
      m_dateFmt{_config.dateFmt}
{
    validLogFmt(this->m_logFmt);
    validDateFmt(this->m_dateFmt);

    this->setColorMode(_config.colorMode);
    this->setColorTheme(_config.colorTheme);
}

Bool ConsoleSink::setDateFmt(String const& _dateFmt)
{
    if (tryIsValidDateFmt(_dateFmt))
    {
        this->m_dateFmt = _dateFmt;
        return True;
    }

    return False;
}

Bool ConsoleSink::setLogFmt(String const& _logFmt)
{
    if (tryIsValidLogFmt(_logFmt))
    {
        this->m_logFmt = _logFmt;
        return True;
    }

    return False;
}

void ConsoleSink::setColorMode(ConsoleColorMode _colorMode)
{
    this->m_colorMode = _colorMode;

    switch (this->m_colorMode)
    {
        case ConsoleColorMode::Auto:

#ifdef NAMICA_PLATFORM_WINDOWS
            this->m_colorEnabled = tryEnableAnsiColor(
                GetStdHandle(m_os == &std::cout ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE));
#else
            this->m_colorEnabled = False;
#endif
            break;
        case ConsoleColorMode::Always:
            this->m_colorEnabled = True;
            break;
        case ConsoleColorMode::Never:
            this->m_colorEnabled = False;
            break;
    }
}

void ConsoleSink::setColorTheme(ConsoleColorTheme const& _colorTheme)
{
    this->m_logColorIndex[0] = static_cast<UInt8>(_colorTheme.traceColor);
    this->m_logColorIndex[1] = static_cast<UInt8>(_colorTheme.debugColor);
    this->m_logColorIndex[2] = static_cast<UInt8>(_colorTheme.infoColor);
    this->m_logColorIndex[3] = static_cast<UInt8>(_colorTheme.warringColor);
    this->m_logColorIndex[4] = static_cast<UInt8>(_colorTheme.errorColor);
    this->m_logColorIndex[5] = static_cast<UInt8>(_colorTheme.criticalColor);
}

Bool ConsoleSink::write(LogRecord const& _record)
{
    UInt8 const levelIndex{static_cast<UInt8>(_record.logLevel)};
    String const timeStr{_record.timePoint.getStr(this->m_dateFmt)};
    String const& levelStr{getLogLevelStr(_record.logLevel)};

    if (this->m_colorEnabled)
    {
        *(this->m_os) << levelPreColor[this->m_logColorIndex[levelIndex]]
                      << std::vformat(this->m_logFmt,
                                      std::make_format_args(timeStr, levelStr, _record.message))
                      << levelPostColor[this->m_logColorIndex[levelIndex]] << "\n";
    }
    else
    {
        *(this->m_os) << std::vformat(this->m_logFmt,
                                      std::make_format_args(timeStr, levelStr, _record.message))
                      << "\n";
    }

    return this->flush();
}

Bool ConsoleSink::flush()
{
    this->m_os->flush();
    return !(this->m_os->fail());
}

}  // namespace namica