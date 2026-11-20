#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"

namespace namica
{

struct WindowSystemConfig;

class NAMICA_API WindowSystem
{
public:
    CLASS_DISABLE_COPY_MOVE(WindowSystem);
    virtual ~WindowSystem();

public:
    static UniquePtr<WindowSystem> create(WindowSystemConfig const& _config);

public:
};

}  // namespace namica