#pragma once

#include "namica/log/sink/ConsoleSinkConfig.h"
#include "namica/log/LogLevel.h"

namespace namica
{

struct LogConfig
{
    String coreLoggerSource{"Core"};
    Bool haveCoreConsoleSink{True};
    ConsoleSinkConfig coreConsoleSinkConfig{{LogLevel::Trace},
                                            ConsoleTarget::Stdout,
                                            "[{0}][{1}]Core {2}",
                                            NAMICA_TIME_HMS,
                                            ConsoleColorTheme{},
                                            ConsoleColorMode::Auto};

    String appLoggerSource{"App"};
    Bool haveAppConsoleSink{True};
    ConsoleSinkConfig appConsoleSinkConfig{{LogLevel::Trace},
                                           ConsoleTarget::Stdout,
                                           "[{0}][{1}]App {2}",
                                           NAMICA_TIME_HMS,
                                           ConsoleColorTheme{},
                                           ConsoleColorMode::Auto};

    // TODO...
};

}  // namespace namica