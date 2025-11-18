#pragma once

#include "namica/imgui/ImGuiImpl.h"

namespace Namica
{

struct OpenGLImGuiImpl final : public ImGuiImplRenderer
{
    virtual void init() override;
    virtual void shutdown() override;
    virtual void newFrame() override;
    virtual void drawData(ImDrawData* _data) override;
};

}  // namespace Namica