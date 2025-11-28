#include "namica_editor/panel/SceneHierarchyPanel.h"
#include "namica_editor/EditorContext.h"
#include "namica_editor/EditorActions.h"

#include <namica/scene/Entity.h>
#include <namica/scene/EntityIterator.h>
#include <imgui.h>

namespace Namica
{

SceneHierarchyPanel::SceneHierarchyPanel(EditorContext* _context) : EditorPanel{_context}
{
}

SceneHierarchyPanel::~SceneHierarchyPanel()
{
}

void SceneHierarchyPanel::onImGuiRenderer()
{
    ImGui::Begin("场景层次");

    if (m_context->activeScene)
    {
        ImGui::Text("Scene: %s", m_context->activeScene->getName().c_str());
        ImGui::Separator();

        auto& rootEntities{m_context->activeScene->getRootEntities()};
        for (size_t i{0}; i < rootEntities.size(); ++i)
        {
            // 绘制树状节点
            drawEntityNode(rootEntities[i]);
        }

        // 如果点击当前场景面板的空白区域, 将选择entity置为空
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
        {
            m_context->selectionContext = {};
        }

        // 如果在场景层次右键空白区域, 弹出当前窗口的上下文窗口
        // flag: 右键打开 | 悬停在项目上时不返回true，仅在悬停在空白处时返回。
        if (ImGui::BeginPopupContextWindow(
                0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("创建空实体"))
            {
                m_context->activeScene->createEntity();
            }
            ImGui::EndPopup();
        }
    }

    ImGui::End();
}

void SceneHierarchyPanel::drawEntityNode(Entity _entity)
{
    std::string const entityName{_entity.getName()};
    const UUID& entityID{_entity.getUUID()};

    if (m_expandNextFrame == entityID)
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        m_expandNextFrame = 0;
    }

    ImGuiTreeNodeFlags flags =
        (m_context->selectionContext == _entity ? ImGuiTreeNodeFlags_Selected : 0) |
        ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;  // 让标签占满一行，用户更容易点击
    bool opened{ImGui::TreeNodeEx((void*)(uintptr_t)entityID, flags, "%s", entityName.c_str())};
    // 成功点击
    if (ImGui::IsItemClicked())
    {
        m_context->selectionContext = _entity;
    }

    // 右键场景层次节点, 存在相关辅助操作, 比如删除节点
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("添加子实体"))
        {
            Entity childEntity{m_context->activeScene->createEntity("子实体")};
            m_context->activeScene->addChildEntity(_entity, childEntity);
            m_expandNextFrame = entityID;
            m_context->selectionContext = childEntity;
        }
        if (ImGui::MenuItem("复制实体"))
        {
            EditorActions::duplicateEntity();
        }
        if (ImGui::MenuItem("删除实体"))
        {
            m_context->activeScene->destoryEntity(_entity);
        }
        ImGui::EndPopup();
    }

    if (opened)
    {
        for (auto enit{_entity.getChildrenIterator()}; enit.hasNext(); enit.next())
        {
            drawEntityNode(enit.current());
        }
        ImGui::TreePop();
    }
}

}  // namespace Namica