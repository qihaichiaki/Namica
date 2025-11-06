#pragma once

// NAMICA_PLATFORM_WINDOWS
#ifdef _WIN32
#define NAMICA_PLATFORM_WINDOWS
#endif

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