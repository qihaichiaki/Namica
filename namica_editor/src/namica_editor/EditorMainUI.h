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
    void editorPanelInit();
    /// @brief 绘制停靠区间
    void drawDockspace();
    /// @brief 绘制编辑器面板
    void drawPanels(EditorContext& _context);

private:
    void drawMenuBar();

private:
    std::vector<Ref<EditorPanel>> m_panels;
};
}  // namespace Namica