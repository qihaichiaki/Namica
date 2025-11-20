#pragma once

#include <namica/core/Memory.h>
#include "namica_editor/panel/EditorPanel.h"

#include <vector>

namespace Namica
{

class EditorMainUI
{
public:
    /// @brief 编辑器面板初始化
    void editorPanelInit(EditorContext* _context);
    /// @brief 编辑器面板更新
    void onUpdate();
    /// @brief 编辑器面板接收事件
    void onEvent(Event& _event);
    /// @brief 绘制停靠区间
    void drawDockspace();
    /// @brief 绘制编辑器面板
    void drawPanels();

private:
    void drawMenuBar();

private:
    std::vector<Ref<EditorPanel>> m_panels;
};
}  // namespace Namica