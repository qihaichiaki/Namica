#include "namica/core/Log.h"
#include "namica/core/LogConfig.h"

#include <stdexcept>

namespace namica
{

SharedPtr<Logger> Log::s_coreLogger{nullptr};
SharedPtr<Logger> Log::s_appLogger{nullptr};

void Log::init(LogConfig const& _config)
{
    SharedPtr<Logger> coreLogger{};
    SharedPtr<Logger> appLogger{};
    if (s_coreLogger)
    {
        coreLogger = s_coreLogger;
        appLogger = s_appLogger;
    }
    else
    {
        coreLogger = Logger::create();
        appLogger = Logger::create();
    }

    if (!coreLogger->setLogFmt(_config.coreConfig.logFmt) ||
        !appLogger->setLogFmt(_config.appConfig.logFmt))
    {
        throw std::invalid_argument{"invalid logFmt"};
    }

    coreLogger->setLogMinLevel(_config.coreConfig.minLevel);
    appLogger->setLogMinLevel(_config.appConfig.minLevel);

    s_coreLogger = coreLogger;
    s_appLogger = appLogger;
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