#pragma once

#include "namica/core/Application.h"
#include "namica/core/Log.h"

#ifdef NAMICA_PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>
#endif

extern Namica::Scope<Namica::Application> Namica::createApplication();

int main()
{
    // windows use utf-8
#ifdef NAMICA_PLATFORM_WINDOWS
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Namica::Log::init();

    auto app = Namica::createApplication();
    app->run();

    return 0;
}