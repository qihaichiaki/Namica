#pragma once

#include "namica/core/Application.h"
#include "namica/core/Macros.h"

extern namica::Application* namica::createApplication();

int main()
{
    WINDOWS_USE_UTF8

    namica::Application* app = namica::createApplication();
    app->run();
    delete app;

    return 0;
}