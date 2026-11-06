#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"
#include "namica/log/Logger.h"

namespace namica
{

struct LogConfig;

class Log final
{
public:
    /**
     * @brief 获取Core Logger对象
     * @note [x:x:x][info]Core: xxx
     */
    NAMICA_API static Logger& getCoreLogger();

    /**
     * @brief 获取App Logger对象
     * @note [x:x:x][info]App: xxx
     */
    NAMICA_API static Logger& getAppLogger();

private:
    /**
     * @brief log系统初始化
     * @throw invalid_argument 部分参数初始化失败
     */
    static void init(LogConfig const& _config);

    static void clear();

private:
    static SharedPtr<Logger> s_coreLogger;  // namica app内部使用logger
    static SharedPtr<Logger> s_appLogger;   // 继承自namica的app使用logger

    friend class Application;
};

}  // namespace namica

// 日志便捷宏

// 日志便捷LOG
#ifdef NAMICA_LOG_DEBUG

#define NAMICA_CORE_TRACE(...) \
    namica::Log::getCoreLogger().log(namica::LogLevel::Trace, __VA_ARGS__)
#define NAMICA_APP_TRACE(...) namica::Log::getAppLogger().log(namica::LogLevel::Trace, __VA_ARGS__)

#define NAMICA_CORE_DEBUG(...) \
    namica::Log::getCoreLogger().log(namica::LogLevel::Debug, __VA_ARGS__)
#define NAMICA_APP_DEBUG(...) namica::Log::getAppLogger().log(namica::LogLevel::Debug, __VA_ARGS__)

#else

#define NAMICA_CORE_TRACE(...) (void)0
#define NAMICA_APP_TRACE(...) (void)0

#define NAMICA_CORE_DEBUG(...) (void)0
#define NAMICA_APP_DEBUG(...) (void)0

#endif

#define NAMICA_CORE_INFO(...) namica::Log::getCoreLogger().log(namica::LogLevel::Info, __VA_ARGS__)
#define NAMICA_APP_INFO(...) namica::Log::getAppLogger().log(namica::LogLevel::Info, __VA_ARGS__)

#define NAMICA_CORE_WARNING(...) \
    namica::Log::getCoreLogger().log(namica::LogLevel::Warning, __VA_ARGS__)
#define NAMICA_APP_WARNING(...) \
    namica::Log::getAppLogger().log(namica::LogLevel::Warning, __VA_ARGS__)

#define NAMICA_CORE_ERROR(...) \
    namica::Log::getCoreLogger().log(namica::LogLevel::Error, __VA_ARGS__)
#define NAMICA_APP_ERROR(...) namica::Log::getAppLogger().log(namica::LogLevel::Error, __VA_ARGS__)

#define NAMICA_CORE_CRITICAL(...) \
    namica::Log::getCoreLogger().log(namica::LogLevel::Critical, __VA_ARGS__)
#define NAMICA_APP_CRITICAL(...) \
    namica::Log::getAppLogger().log(namica::LogLevel::Critical, __VA_ARGS__)

// 日志便捷Assert

#ifdef NAMICA_ENABLE_ASSERTS

#ifdef NAMICA_PLATFORM_WINDOWS
#define NAMICA_DEBUG_BREAK __debugbreak()
#else
#define NAMICA_DEBUG_BREAK
#endif

#define NAMICA_STRINGIFY_IMPL(x) #x
#define NAMICA_STRINGIFY(x) NAMICA_STRINGIFY_IMPL(x)

#define NAMICA_CORE_ASSERT(condition)                                                \
    do                                                                               \
    {                                                                                \
        if (!(condition))                                                            \
        {                                                                            \
            ::namica::Log::getCoreLogger().log(namica::LogLevel::Error,              \
                                               "Assertion Failed (" __FILE__         \
                                               ":" NAMICA_STRINGIFY(__LINE__) ") "); \
            NAMICA_DEBUG_BREAK;                                                      \
        }                                                                            \
    } while (0)

#define NAMICA_APP_ASSERT(condition)                                                \
    do                                                                              \
    {                                                                               \
        if (!(condition))                                                           \
        {                                                                           \
            ::namica::Log::getAppLogger().log(namica::LogLevel::Error,              \
                                              "Assertion Failed (" __FILE__         \
                                              ":" NAMICA_STRINGIFY(__LINE__) ") "); \
            NAMICA_DEBUG_BREAK;                                                     \
        }                                                                           \
    } while (0)

#else

#define NAMICA_CORE_ASSERT(condition) (void)0
#define NAMICA_APP_ASSERT(condition) (void)0

#endif