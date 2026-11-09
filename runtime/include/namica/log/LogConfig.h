#pragma once

#include "namica/log/sink/ConsoleSinkConfig.h"
#include "namica/log/LogLevel.h"

namespace namica
{

struct LogConfig
{
    String coreLoggerSource{"Core"};
    Bool haveCoreConsoleSink{True};
    ConsoleSinkConfig coreConsoleSinkConfig{LogLevel::Trace,
                                            NAMICA_DEFAULT_LOG_FMT,
                                            NAMICA_TIME_HMS,
                                            ConsoleTarget::Stdout,
                                            ConsoleColorTheme{},
                                            ConsoleColorMode::Auto};

    String appLoggerSource{"App"};
    Bool haveAppConsoleSink{True};
    ConsoleSinkConfig appConsoleSinkConfig{LogLevel::Trace,
                                           NAMICA_DEFAULT_LOG_FMT,
                                           NAMICA_TIME_HMS,
                                           ConsoleTarget::Stdout,
                                           ConsoleColorTheme{},
                                           ConsoleColorMode::Auto};

    // TODO...
};

}  // namespace namica