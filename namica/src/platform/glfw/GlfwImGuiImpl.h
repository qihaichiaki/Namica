#pragma once

#include "namica/imgui/ImGuiImpl.h"

namespace Namica
{

struct GlfwImGuiImpl final : public ImGuiImplWindow
{
    virtual void init(Window const& _windowHandle) override;
    virtual void shutdown() override;
    virtual void newFrame() override;
};
}  // namespace Namica