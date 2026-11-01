#pragma once

// 平台宏

// NAMICA_PLATFORM_WINDOWS
#ifdef _WIN32
#define NAMICA_PLATFORM_WINDOWS
#endif

// 导出宏

// NAMICA_API
#ifdef NAMICA_PLATFORM_WINDOWS
#ifdef NAMICA_BUILD_DLL
#define NAMICA_API __declspec(dllexport)
#else
#define NAMICA_API __declspec(dllimport)
#endif
#else
#error 当前引擎仅仅支持Windows平台!
#endif

// 辅助宏

// 辅助设置位数
#define BIT(X) (1 << X)

// 禁当前类copy或者move
#define CLASS_DISABLE_COPY_MOVE(Class)       \
    Class(const Class&) = delete;            \
    Class(Class&&) = delete;                 \
    Class& operator=(const Class&) = delete; \
    Class& operator=(const Class&&) = delete;
