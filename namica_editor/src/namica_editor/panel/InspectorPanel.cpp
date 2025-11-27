#include "namica_editor/panel/InspectorPanel.h"
#include "namica_editor/EditorContext.h"

#include <namica/imgui/ImGuiLayer.h>
#include <namica/scene/Entity.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Namica
{

InspectorPanel::InspectorPanel(EditorContext* _context) : EditorPanel{_context}
{
}

InspectorPanel::~InspectorPanel()
{
}

static bool drawVec3Contol(std::string const& _label,
                           glm::vec3& _values,
                           float _resetValue = 0.0f,
                           char const* _format = "%.2f",
                           float _columnWidth = 55.0f)
{
    auto& io = ImGui::GetIO();
    // 绘制当前组件使用粗体
    auto bold_font = io.Fonts->Fonts[0];

    bool isCol = false;

    ImGui::PushID(_label.c_str());
    ImGui::Columns(2);                       // 开始设置为2列
    ImGui::SetColumnWidth(0, _columnWidth);  // 设置第一列的宽度
    ImGui::Text("%s", _label.c_str());
    ImGui::NextColumn();                                     // 开始添加下一行
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());  // 设置多个item的宽度
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.0f, 0.0f});
    auto gimgui = GetImGuiContextPtr();
    float line_height = gimgui->FontSize + gimgui->Style.FramePadding.y * 2.0f;  // 计算行高
    ImVec2 button_size{line_height + 3.0f, line_height};
    // 创建按钮 x, y, z
    // 按钮颜色
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushFont(bold_font);
    if (ImGui::Button("X", button_size))
    {
        _values.x = _resetValue;
        isCol = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &_values.x, 0.1f, 0.0f, 0.0f, _format))
    {
        isCol = true;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Y", button_size))
    {
        _values.y = _resetValue;
        isCol = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &_values.y, 0.1f, 0.0f, 0.0f, _format))
    {
        isCol = true;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Z", button_size))
    {
        _values.z = _resetValue;
        isCol = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &_values.z, 0.1f, 0.0f, 0.0f, _format))
    {
        isCol = true;
    }
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();

    return isCol;
}

template <typename T, typename UIFunction>
static void drawComponent(std::string const& _name,
                          Entity _entity,
                          UIFunction _uiFunction,
                          bool _isNoDelete = false)
{
    // 默认展开 | 运行项目重叠（目的时在treenode这一行继续添加控件） | 整一行收标签控制 |
    // 整一行被填充背景色(和当前行元素高度自适应) | 添加框的内边距
    ImGuiTreeNodeFlags const tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth |
        ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

    if (_entity.hasComponent<T>())
    {
        auto& component = _entity.getComponent<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        auto gimgui = GetImGuiContextPtr();
        float line_height = gimgui->FontSize + gimgui->Style.FramePadding.y * 2.0f;  // 计算行高
        ImGui::Separator();
        bool is_open =
            ImGui::TreeNodeEx((void*)typeid(T).hash_code(), tree_node_flags, "%s", _name.c_str());
        bool is_render_button = false;
        // 判断鼠标是否悬浮在这个 TreeNode 的矩形区域上上
        if (bool hovered =
                ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
        {
            // 鼠标悬浮在 TreeNode 上
            is_render_button = true;
        }
        ImGui::PopStyleVar();

        bool remove_component = false;
        if (is_render_button)
        {
            ImGui::SameLine(contentRegionAvailable.x - line_height * 0.5f);
            if (ImGui::Button("•••", ImVec2{line_height, line_height}))  // 扩展按钮
            {
                ImGui::OpenPopup("ComponentSettings");
            }
        }

        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (ImGui::MenuItem("删除组件", 0, false, !_isNoDelete))
            {
                remove_component = true;
            }
            ImGui::EndPopup();
        }

        if (is_open)
        {
            // 独特的组件渲染信息内容
            _uiFunction(component);
            ImGui::TreePop();
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 2.0f));  // 空两倍默认间距
        }

        if (remove_component)
        {
            _entity.removeComponent<T>();
        }
    }
}

void InspectorPanel::onImGuiRenderer()
{
    ImGui::Begin("实体属性");
    if (m_context->selectionContext)
    {
        drawComponents(m_context->selectionContext);
    }
    ImGui::End();
}

void InspectorPanel::drawComponents(Entity entity)
{
    ImGui::PushID(static_cast<int>(entity.getUUID()));

    // tag
    std::string& entityName{entity.getName()};
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strcpy_s(buffer, sizeof(buffer), entityName.c_str());

    if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
    {
        entityName = std::string{buffer};
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::Button("添加组件"))
    {
        // 添加组件打开对应id的上下文窗口
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent"))
    {
        if (ImGui::MenuItem(
                "Camera", 0, false, !m_context->selectionContext.hasComponent<CameraComponent>()))
        {
            m_context->selectionContext.addComponent<CameraComponent>();
            // ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("SpriteRenderer",
                            0,
                            false,
                            !m_context->selectionContext.hasComponent<SpriteRendererComponent>()))
        {
            m_context->selectionContext.addComponent<SpriteRendererComponent>();
        }
        if (ImGui::MenuItem("CircleRenderer",
                            0,
                            false,
                            !m_context->selectionContext.hasComponent<CircleRendererComponent>()))
        {
            m_context->selectionContext.addComponent<CircleRendererComponent>();
        }
        if (ImGui::MenuItem("Rigidbody2D",
                            0,
                            false,
                            !m_context->selectionContext.hasComponent<Rigidbody2DComponent>()))
        {
            m_context->selectionContext.addComponent<Rigidbody2DComponent>();
        }
        if (ImGui::MenuItem("BoxCollider2D",
                            0,
                            false,
                            !m_context->selectionContext.hasComponent<BoxCollider2DComponent>() &&
                                m_context->selectionContext.hasComponent<Rigidbody2DComponent>()))
        {
            m_context->selectionContext.addComponent<BoxCollider2DComponent>();
        }
        if (ImGui::MenuItem(
                "CircleCollider2D",
                0,
                false,
                !m_context->selectionContext.hasComponent<CircleCollider2DComponent>() &&
                    m_context->selectionContext.hasComponent<Rigidbody2DComponent>()))
        {
            m_context->selectionContext.addComponent<CircleCollider2DComponent>();
        }
        ImGui::EndPopup();
    }
    ImGui::PopItemWidth();
    ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 2.0f));  // 空两倍默认间距

    // transform
    drawComponent<TransformComponent>(
        "Transform",
        entity,
        [](TransformComponent& transform) {
            drawVec3Contol("位置", transform.translation);
            // 转角度
            glm::vec3 d_roation = glm::degrees(transform.rotation);
            if (drawVec3Contol("旋转", d_roation, 0.0f, "%.1f°"))
            {
                transform.rotation = glm::radians(d_roation);
            }
            drawVec3Contol("缩放", transform.scale, 1.0f);
        },
        true);

    // Camera
    drawComponent<CameraComponent>("Camera", entity, [](CameraComponent& camera) {
        ImGui::Checkbox("渲染", &camera.primary);
        const char* projection_type_strings[] = {"透视", "正交"};
        const char* current_projection_type_string =
            projection_type_strings[(int)camera.camera.getProjectionType()];
        if (ImGui::BeginCombo("投影类型", current_projection_type_string))
        {
            for (int i = 0; i < 2; ++i)
            {
                bool is_selected = current_projection_type_string == projection_type_strings[i];
                if (ImGui::Selectable(projection_type_strings[i], is_selected))
                {
                    current_projection_type_string = projection_type_strings[i];
                    camera.camera.setProjectionType((Camera::ProjectionType)i);
                }

                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (strcmp(current_projection_type_string, "透视") == 0)
        {
            float fov_degree = glm::degrees(camera.camera.getPerspectiveFOV());
            if (ImGui::DragFloat("垂直视野", &fov_degree))
            {
                camera.camera.setPerspectiveFOV(glm::radians(fov_degree));
            }
            float p_near = camera.camera.getPerspectiveNearClip();
            if (ImGui::DragFloat("近点", &p_near))
            {
                camera.camera.setPerspectiveNearClip(p_near);
            }
            float p_far = camera.camera.getPerspectiveFarClip();
            if (ImGui::DragFloat("远点", &p_far))
            {
                camera.camera.setPerspectiveFarClip(p_far);
            }
        }
        else
        {
            float o_size = camera.camera.getOrthographicSize();
            if (ImGui::DragFloat("正交大小", &o_size))
            {
                camera.camera.setOrthographicSize(o_size);
            }
            float o_near = camera.camera.getOrthographicNearClip();
            if (ImGui::DragFloat("近点", &o_near))
            {
                camera.camera.setOrthographicNearClip(o_near);
            }
            float o_far = camera.camera.getOrthographicFarClip();
            if (ImGui::DragFloat("远点", &o_far))
            {
                camera.camera.setOrthographicFarClip(o_far);
            }
        }

        ImGui::Checkbox("固定纵横比", &camera.fixedAspectRatio);
    });

    // SpriteRenderer
    drawComponent<SpriteRendererComponent>(
        "Sprite", entity, [](SpriteRendererComponent& spriteRenderer) {
            ImGui::ColorEdit4("颜色", glm::value_ptr(spriteRenderer.color));

            ImGui::Button("纹理");
            // if (ImGui::BeginDragDropTarget())
            // {
            //     const ImGuiPayload* item_data = ImGui::AcceptDragDropPayload("CONTEXT_ITEM");
            //     if (item_data)
            //     {
            //         std::string file_path = (char*)item_data->Data;
            //         spriteRenderer.texture = Texture2D::create(file_path);
            //     }
            //     ImGui::EndDragDropTarget();
            // }

            ImGui::DragFloat("平铺因子", &spriteRenderer.tilingFactor, 1.0f, 0.0f, 1000.0f);
        });

    // CircleRenderer
    drawComponent<CircleRendererComponent>(
        "Circle", entity, [](CircleRendererComponent& circleRenderer) {
            ImGui::ColorEdit4("颜色", glm::value_ptr(circleRenderer.color));
            ImGui::DragFloat("厚度", &circleRenderer.thickness, 0.025f, 0.0f, 1.0f);
            ImGui::DragFloat("模糊效果", &circleRenderer.fade, 0.0005f, 0.0f, 1.0f);
        });

    // Rigidbody2D
    drawComponent<Rigidbody2DComponent>(
        "Rigidbody2D", entity, [](Rigidbody2DComponent& rigidbody2D) {
            const char* body_type_strings[] = {"static", "dynamic", "kinematic"};
            const char* current_body_type_string = body_type_strings[(int)rigidbody2D.type];
            if (ImGui::BeginCombo("刚体类型", current_body_type_string))
            {
                for (int i = 0; i < 3; ++i)
                {
                    bool is_selected = current_body_type_string == body_type_strings[i];
                    if (ImGui::Selectable(body_type_strings[i], is_selected))
                    {
                        current_body_type_string = body_type_strings[i];
                        rigidbody2D.type = (Physics2D::RigidbodyType)i;
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Checkbox("固定z轴旋转", &rigidbody2D.fixedRotation);
        });

    // BoxCollider2D
    drawComponent<BoxCollider2DComponent>(
        "BoxCollider2D", entity, [](BoxCollider2DComponent& boxCollider2D) {
            ImGui::DragFloat2("偏移", glm::value_ptr(boxCollider2D.offset), 0.1f);
            ImGui::DragFloat2("大小", glm::value_ptr(boxCollider2D.size), 0.1f);
            ImGui::DragFloat("密度", &boxCollider2D.physicalMaterials.density, 0.01f);
            ImGui::DragFloat(
                "摩擦力", &boxCollider2D.physicalMaterials.friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat(
                "反弹系数", &boxCollider2D.physicalMaterials.restitution, 0.01f, 0.0f, 1.0f);
            // ImGui::DragFloat("反弹恢复阈值",
            //                  &boxCollider2D.physicalMaterials.restitutionThreshold,
            //                  0.01f,
            //                  0.0f,
            //                  1.0f,
            //                  "%.3f",
            //                  ImGuiSliderFlags_NoInput);
        });

    // CircleCollider2D
    drawComponent<CircleCollider2DComponent>(
        "CircleCollider2D", entity, [](CircleCollider2DComponent& circleCollider2D) {
            ImGui::DragFloat2("偏移", glm::value_ptr(circleCollider2D.offset), 0.1f);
            ImGui::DragFloat("半径", &circleCollider2D.radius, 0.1f);
            ImGui::DragFloat("密度", &circleCollider2D.physicalMaterials.density, 0.01f);
            ImGui::DragFloat(
                "摩擦力", &circleCollider2D.physicalMaterials.friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat(
                "反弹系数", &circleCollider2D.physicalMaterials.restitution, 0.01f, 0.0f, 1.0f);
            // ImGui::DragFloat("反弹恢复阈值",
            //                  &circleCollider2D.physicalMaterials.restitutionThreshold,
            //                  0.01f,
            //                  0.0f,
            //                  1.0f,
            //                  "%.3f",
            //                  ImGuiSliderFlags_NoInput);
        });

    ImGui::PopID();
}

}  // namespace Namica