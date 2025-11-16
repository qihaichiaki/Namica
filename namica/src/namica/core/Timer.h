#pragma once

#include "namica/core/Base.h"
#include <chrono>

namespace Namica
{
class Timer
{
public:
    NAMICA_API Timer()
    {
        reset();
    }

    NAMICA_API ~Timer()
    {
    }

    NAMICA_API void reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    NAMICA_API float elapsed()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::high_resolution_clock::now() - m_Start)
                   .count() *
            0.001f * 0.001f * 0.001f;  // 返回s
    }

    NAMICA_API float elapsedMillis()
    {
        return elapsed() * 1000.0f;  // ms
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};
}  // namespace Namica