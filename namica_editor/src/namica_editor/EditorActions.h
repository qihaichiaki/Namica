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

private:
    static EditorContext* s_context;
};

}  // namespace Namica