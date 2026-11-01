#include "runtime/utils/TestUtils.h"

#include <namica/core/Logger.h>

TEST_F(NamicaRuntimeTest, logger)
{
    namica::SharedPtr<namica::Logger> logger{namica::Logger::create()};

    logger->log(namica::LogLevel::Debug, "你好世界!");
}