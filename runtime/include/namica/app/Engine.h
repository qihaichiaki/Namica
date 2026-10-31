#pragma once

#include "namica/core/Base.h"

namespace namica
{

/**
 * @brief 引擎对象
 *
 */
class Engine
{
public:
    NAMICA_API static Engine* getInstance();

private:
    Engine();
    Engine(Engine const&);
    ~Engine();
};

}  // namespace namica