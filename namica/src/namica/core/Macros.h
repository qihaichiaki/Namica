#pragma once

#include "namica/core/Base.h"

// WINDOWS_USE_UTF8
#ifdef NAMICA_PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>

#define WINDOWS_USE_UTF8         \
    SetConsoleOutputCP(CP_UTF8); \
    SetConsoleCP(CP_UTF8);
#else
#define WINDOWS_USE_UTF8
#endif