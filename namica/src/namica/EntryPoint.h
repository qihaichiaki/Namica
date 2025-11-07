#pragma once

#include "namica/core/Application.h"
#include "namica/core/Macros.h"
#include "namica/core/Log.h"

extern Namica::Scope<Namica::Application> Namica::createApplication();

int main()
{
    WINDOWS_USE_UTF8

    Namica::Log::init();

    auto app = Namica::createApplication();
    app->run();

    return 0;
}