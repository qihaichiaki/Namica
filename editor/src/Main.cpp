#include <namica/app/Application.h>
#include <namica/app/ApplicationConfig.h>
#include <namica/log/Log.h>

int main()
{
    namica::ApplicationConfig config{};
    config.name = "NamicaEditor";
    config.logConfig.haveFileSink = namica::True;
    namica::SharedPtr<namica::Application> app{new namica::Application{config}};
    NAMICA_APP_INFO("日志记录, 当前App配置名字为{}", config.name);

    app->run();

    return 0;
}