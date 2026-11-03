#include "namica/core/Log.h"

namespace namica
{

SharedPtr<Logger> Log::s_coreLogger{nullptr};
SharedPtr<Logger> Log::s_appLogger{nullptr};

void Log::init()
{
    if (!s_coreLogger)
    {
        s_coreLogger = Logger::create();
        s_appLogger = Logger::create();
    }

    s_coreLogger->setLogFmt("[{0}][{1}]Core: {2}");
    s_appLogger->setLogFmt("[{0}][{1}]App: {2}");
    // TODO: LogConfig
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