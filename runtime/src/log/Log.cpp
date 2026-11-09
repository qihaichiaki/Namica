#include "namica/log/Log.h"
#include "namica/log/LogConfig.h"
#include "namica/log/sink/ConsoleSink.h"
#include "namica/log/sink/FileSink.h"

namespace namica
{

SharedPtr<Logger> Log::s_coreLogger{nullptr};
SharedPtr<Logger> Log::s_appLogger{nullptr};

void Log::init(LogConfig const& _config)
{
    if (s_coreLogger)
    {
        return;
    }

    SharedPtr<Logger> coreLogger{};
    SharedPtr<Logger> appLogger{};

    coreLogger = Logger::create(_config.coreLoggerSource);
    appLogger = Logger::create(_config.appLoggerSource);

    if (_config.haveConsoleSink)
    {
        auto consoleSink{ConsoleSink::create(_config.consoleSinkConfig)};
        coreLogger->addSink(consoleSink);
        appLogger->addSink(consoleSink);
    }

    if (_config.haveFileSink)
    {
        auto fileSink{FileSink::create(_config.fileSinkConfig)};
        coreLogger->addSink(fileSink);
        appLogger->addSink(fileSink);
    }

    s_coreLogger = coreLogger;
    s_appLogger = appLogger;
}

void Log::clear()
{
    if (!s_coreLogger)
    {
        return;
    }

    s_coreLogger->flush();
    s_appLogger->flush();

    s_coreLogger.reset();
    s_appLogger.reset();
}

Logger& Log::getCoreLogger()
{
    return *s_coreLogger;
}

Logger& Log::getAppLogger()
{
    return *s_appLogger;
}

}  // namespace namica