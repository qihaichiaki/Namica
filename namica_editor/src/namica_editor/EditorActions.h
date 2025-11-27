#pragma once

namespace Namica
{

struct EditorContext;

class EditorActions
{
public:
    // 编辑器行为初始化
    static void init(EditorContext* _context);

    // 复制当前选中实体
    static void duplicateEntity();

    // 进入播放模式 -> runtime模式
    static void enterPalyMode();

    // 推出播放模式 -> editor模式
    static void exitPalyMode();

private:
    static EditorContext* s_context;
};

}  // namespace Namica