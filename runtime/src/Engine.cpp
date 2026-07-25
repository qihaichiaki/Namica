#include "namica/Engine.h"

namespace namica
{

Engine* Engine::getInstance()
{
    static Engine instance{};
    return &instance;
}

Engine::Engine()
{
}

Engine::Engine(Engine const&)
{
}

Engine::~Engine()
{
}

}  // namespace namica