#pragma once

#include "namica/core/Base.h"
#include "namica/core/Layer.h"
#include "namica/core/Memory.h"

namespace Namica
{

struct ImGuiImplWindow;
struct ImGuiImplRenderer;

class ImGuiLayer final : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer();

    void onAttach() override;
    void onDetach() override;
    void onEvent(Event&) override;
    void onImGuiRender() override;
    void setBlockEvents(bool _blockEvents);
    void setDarkThemeColors();
    /// @brief 初始化imgui帧
    void begin();
    /// @brief 结束imgui帧
    void end();

private:
    bool m_blockEvents{true};
    Scope<ImGuiImplWindow> m_imguiImplWindow{nullptr};
    Scope<ImGuiImplRenderer> m_imGuiImplRenderer{nullptr};
};

}  // namespace Namica

struct ImGuiContext;
NAMICA_API inline ImGuiContext* GetImGuiContextPtr();