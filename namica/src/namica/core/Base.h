#pragma once

// NAMICA_PLATFORM_WINDOWS
#ifdef _WIN32
#ifdef _WIN64
#define NAMICA_PLATFORM_WINDOWS
#else
#error Windows平台不支持非x64架构!
#endif
#endif

// NAMICA_API
#ifdef NAMICA_PLATFORM_WINDOWS
#ifdef NAMICA_BUILD_DLL
#define NAMICA_API __declspec(dllexport)
#else
#define NAMICA_API __declspec(dllimport)
#endif
#else
#error 当前引擎仅仅支持Windows-x64平台!
#endif

// WINDOWS_USE_UTF8
// #ifdef _WIN32
// #define NOMINMAX
// #include <windows.h>
// #define WINDOWS_USE_UTF8         \
//     SetConsoleOutputCP(CP_UTF8); \
//     SetConsoleCP(CP_UTF8);
// #else
// #define WINDOWS_USE_UTF8
// #endif