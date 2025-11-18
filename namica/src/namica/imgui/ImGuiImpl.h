#pragma once

#include "namica/core/Memory.h"

struct ImDrawData;

namespace Namica
{

class Window;

struct ImGuiImplWindow
{
    virtual ~ImGuiImplWindow() = default;

    virtual void init(Window const& _windowHandle) = 0;
    virtual void shutdown() = 0;
    virtual void newFrame() = 0;

    static Scope<ImGuiImplWindow> create();
};

struct ImGuiImplRenderer
{
    virtual ~ImGuiImplRenderer() = default;

    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual void newFrame() = 0;
    virtual void drawData(ImDrawData* _data) = 0;

    static Scope<ImGuiImplRenderer> create();
};

}  // namespace Namica