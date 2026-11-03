#include <namica/app/Application.h>
#include <namica/app/ApplicationConfig.h>

int main()
{
    namica::ApplicationConfig config{};
    namica::SharedPtr<namica::Application> app{new namica::Application{config}};

    app->run();

    return 0;
}