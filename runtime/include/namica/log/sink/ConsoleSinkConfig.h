#pragma once

#include "namica/core/Types.h"
#include "namica/log/sink/TextSinkConfig.h"

namespace namica
{

enum class ConsoleTarget
{
    Stdout,
    Stderr,
};

enum class ConsoleColor
{
    Default,
    Cyan,
    Green,
    Yellow,
    Red
};

struct ConsoleColorTheme
{
    ConsoleColor traceColor{ConsoleColor::Default};
    ConsoleColor debugColor{ConsoleColor::Green};
    ConsoleColor infoColor{ConsoleColor::Cyan};
    ConsoleColor warringColor{ConsoleColor::Yellow};
    ConsoleColor errorColor{ConsoleColor::Red};
    ConsoleColor criticalColor{ConsoleColor::Red};
};

enum class ConsoleColorMode
{
    Auto,    // 检测输出目标，适合显示颜色时才启用
    Always,  // 强制输出颜色表示
    Never,   // 不添加颜色显示
};

struct ConsoleSinkConfig : TextSinkConfig
{
    ConsoleTarget target{ConsoleTarget::Stdout};
    ConsoleColorTheme colorTheme{};
    ConsoleColorMode colorMode{ConsoleColorMode::Auto};

    ConsoleSinkConfig() = default;
    ConsoleSinkConfig(LogLevel _minLevel,
                      String const& _logFmt,
                      String const& _dateFmt,
                      ConsoleTarget _target,
                      ConsoleColorTheme const& _colorTheme,
                      ConsoleColorMode _colorMode)
        : TextSinkConfig{_minLevel, _logFmt, _dateFmt},
          target{_target},
          colorTheme{_colorTheme},
          colorMode{_colorMode}
    {
    }
};

}  // namespace namica