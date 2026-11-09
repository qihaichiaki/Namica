#include "namica/log/sink/ConsoleSink.h"
#include "namica/core/Base.h"
#include "namica/log/LogRecord.h"

#include <iostream>

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

SharedPtr<ConsoleSink> ConsoleSink::create(ConsoleSinkConfig const& _config)
{
    return SharedPtr<ConsoleSink>{new ConsoleSink{_config}};
}

ConsoleSink::ConsoleSink(ConsoleSinkConfig const& _config)
    : TextSink{_config}, m_os{_config.target == ConsoleTarget::Stdout ? &std::cout : &std::cerr}
{
    this->setColorMode(_config.colorMode);
    this->setColorTheme(_config.colorTheme);
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
    if (this->m_colorEnabled)
    {
        UInt8 const levelIndex{static_cast<UInt8>(_record.logLevel)};
        *(this->m_os) << levelPreColor[this->m_logColorIndex[levelIndex]]
                      << this->formatRecord(_record)
                      << levelPostColor[this->m_logColorIndex[levelIndex]] << "\n";
    }
    else
    {
        *(this->m_os) << this->formatRecord(_record) << "\n";
    }

    return this->flush();
}

Bool ConsoleSink::flush()
{
    this->m_os->flush();
    return !(this->m_os->fail());
}

}  // namespace namica