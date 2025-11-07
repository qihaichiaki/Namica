#include "namica/core/Log.h"

namespace Namica
{

Ref<Logger> Log::s_coreLogger{nullptr};
Ref<Logger> Log::s_appLogger{nullptr};

void Log::init()
{
    s_coreLogger = Logger::create();
    s_appLogger = Logger::create();

    s_coreLogger->setLogFmt("[{0}][{1}]Namica: {2}");
    s_appLogger->setLogFmt("[{0}][{1}]App: {2}");
}

Ref<Logger> const& Log::getLogger(Type _type)
{
    return _type == Log::Type::Core ? s_coreLogger : s_appLogger;
}

}  // namespace Namica