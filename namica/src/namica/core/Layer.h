#pragma once

#include "namica/core/Base.h"

namespace Namica
{

class Event;
class NAMICA_API Layer
{
public:
    Layer(char const* _layerName = "layer");
    virtual ~Layer();

    /**
     * @brief 被push入LayerStack会被调用的方法
     */
    virtual void onAttach();

    /**
     * @brief 被pop出LayerStack会被调用的方法
     */
    virtual void onDetach();

    /**
     * @brief Layer层的核心更新逻辑执行
     */
    virtual void onUpdate();

    /**
     * @brief Layer层接收事件处理
     */
    virtual void onEvent(Event& _event);

    /**
     * @brief Layer层处理imgui渲染
     */
    virtual void onImGuiRender();

    /**
     * @brief 获取Layer层的名字
     */
    char const* getName() const noexcept;

private:
    char const* m_layerName;
};

}  // namespace Namica