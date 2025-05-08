#include "application.h"
#include <common/time.hpp>
#include <thread>

namespace namica
{

void Application::run()
{
    auto& time = Time::getInstance();

    init();
    time.startTick();
    while (is_running) {
        time.frameTick();

        beginFrame();
        processEvents();
        update();
        render();
        endFrame();

        auto slee_duration = time.sleepDuration();
        if (slee_duration > std::chrono::nanoseconds{0}) {
            std::this_thread::sleep_for(slee_duration);
        }
    }
}

}  // namespace namica