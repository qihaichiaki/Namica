#pragma once

#include "namica/core/Base.h"
#include "namica/core/Logger.h"

namespace Namica
{

class Log final
{
public:
    enum class Type
    {
        Core,
        App
    };

public:
    /**
     * @brief 日志初始化
     */
    NAMICA_API static void init();

    /**
     * @brief Get the Logger object
     *
     * @param _type 日志类型
     */
    NAMICA_API static Ref<Logger> const& getLogger(Type _type);

private:
    static Ref<Logger> s_coreLogger;
    static Ref<Logger> s_appLogger;
};

// 日志便捷使用宏

// Debug
#ifdef NAMICA_DEBUG
#define NAMICA_CORE_DEBUG(...) \
    ::Namica::Log::getLogger(::Namica::Log::Type::Core)->log(Namica::LogLevel::Debug, __VA_ARGS__)
#define NAMICA_APP_DEBUG(...) \
    ::Namica::Log::getLogger(::Namica::Log::Type::App)->log(Namica::LogLevel::Debug, __VA_ARGS__)
#else
#define NAMICA_CORE_DEBUG(...)
#define NAMICA_APP_DEBUG(...)
#endif

// Info
#define NAMICA_CORE_INFO(...) \
    ::Namica::Log::getLogger(::Namica::Log::Type::Core)->log(Namica::LogLevel::Info, __VA_ARGS__)
#define NAMICA_APP_INFO(...) \
    ::Namica::Log::getLogger(::Namica::Log::Type::App)->log(Namica::LogLevel::Info, __VA_ARGS__)

// Warn
#define NAMICA_CORE_WARN(...) \
    ::Namica::Log::getLogger(::Namica::Log::Type::Core)->log(Namica::LogLevel::Warn, __VA_ARGS__)
#define NAMICA_APP_WARN(...) \
    ::Namica::Log::getLogger(::Namica::Log::Type::App)->log(Namica::LogLevel::Warn, __VA_ARGS__)

//  Error
#define NAMICA_CORE_ERROR(...) \
    ::Namica::Log::getLogger(::Namica::Log::Type::Core)->log(Namica::LogLevel::Error, __VA_ARGS__)
#define NAMICA_APP_ERROR(...) \
    ::Namica::Log::getLogger(::Namica::Log::Type::App)->log(Namica::LogLevel::Error, __VA_ARGS__)

// Asserts
#ifdef NAMICA_PLATFORM_WINDOWS
#define NAMICA_DEBUG_BREAK __debugbreak()
#else
#define NAMICA_DEBUG_BREAK
#endif

#define NAMICA_STRINGIFY_IMPL(x) #x
#define NAMICA_STRINGIFY(x) NAMICA_STRINGIFY_IMPL(x)

#ifdef NAMICA_ENABLE_ASSERTS
#define NAMICA_CORE_ASSERT(condition, ...)                                           \
    if (!(condition))                                                                \
    {                                                                                \
        ::Namica::Log::getLogger(::Namica::Log::Type::Core)                          \
            ->log(Namica::LogLevel::Error,                                           \
                  "Assertion Failed (" __FILE__ ":" NAMICA_STRINGIFY(__LINE__) ") ", \
                  ##__VA_ARGS__);                                                    \
        NAMICA_DEBUG_BREAK;                                                          \
    }
#define NAMICA_APP_ASSERT(condition, ...)                                            \
    if (!(condition))                                                                \
    {                                                                                \
        ::Namica::Log::getLogger(::Namica::Log::Type::App)                           \
            ->log(Namica::LogLevel::Error,                                           \
                  "Assertion Failed (" __FILE__ ":" NAMICA_STRINGIFY(__LINE__) ") ", \
                  ##__VA_ARGS__);                                                    \
        NAMICA_DEBUG_BREAK;                                                          \
    }
#else
#define NAMICA_CORE_ASSERT(condition, ...) (void)(condition)
#define NAMICA_APP_ASSERT(condition, ...) (void)(condition)
#endif

}  // namespace Namica