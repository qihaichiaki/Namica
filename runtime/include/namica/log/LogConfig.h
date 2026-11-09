#pragma once

#include "namica/log/sink/ConsoleSinkConfig.h"
#include "namica/log/sink/FileSinkConfig.h"
#include "namica/log/LogLevel.h"
#include "namica/io/FileSystem.h"

namespace namica
{

struct LogConfig
{
    String coreLoggerSource{"Core"};
    String appLoggerSource{"App"};

    Bool haveConsoleSink{True};
    ConsoleSinkConfig consoleSinkConfig{LogLevel::Trace,
                                        NAMICA_DEFAULT_LOG_FMT,
                                        NAMICA_TIME_HMS,
                                        ConsoleTarget::Stdout,
                                        ConsoleColorTheme{},
                                        ConsoleColorMode::Auto};

    Bool haveFileSink{False};
    FileSinkConfig fileSinkConfig{LogLevel::Trace,
                                  NAMICA_DEFAULT_LOG_FMT,
                                  NAMICA_TIME_YMD_HMS,
                                  FileSystem::getExecutableFolder() / "Namica.log",
                                  FileOpenMode::Append,
                                  True};
};

}  // namespace namica