#pragma once

#include "namica/core/Base.h"
#include <functional>
#include <string>

namespace Namica
{

enum class EventType
{
    None = 0,
    // 窗口系列
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    // 键盘系列
    KeyPressed,
    KeyReleased,
    KeyTyped,
    // 鼠标系列
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
};

#define BIT(X) (1 << X)
// 事件类别
enum EventCategory
{
    None = 0,
    CategoryWindow = BIT(0),
    CategoryInput = BIT(1),
    CategoryKeyboard = BIT(2),
    CategoryMouse = BIT(3),
    CategoryMouseButton = BIT(4)
};
#undef BIT

class NAMICA_API Event
{
public:
    Event() = default;
    virtual ~Event() = default;

    /**
     * @brief 返回事件对象具体类型
     */
    virtual EventType getType() const noexcept = 0;

    /**
     * @brief 返回事件对象的事件类别
     */
    virtual int getCategoryFlags() const noexcept = 0;

    /**
     * @brief 返回事件的名字
     */
    virtual char const* getName() const noexcept = 0;

    /**
     * @brief 事件自定义消息
     */
    virtual std::string message() const = 0;

    /**
     * @brief 当前事件是否属于传入的事件类别
     *
     * @param _eventCategory 事件类别
     */
    bool isInCategory(EventCategory event_category) const noexcept
    {
        return getCategoryFlags() & event_category;
    }

    bool isHandled{false};  // 事件是否被处理
};

// 事件便捷宏处理
#define EVENT_CLASS_TYPE(_eventType)                      \
    static EventType getStaticType()                      \
    {                                                     \
        return EventType::_eventType;                     \
    }                                                     \
    virtual EventType getType() const noexcept override   \
    {                                                     \
        return getStaticType();                           \
    }                                                     \
    virtual char const* getName() const noexcept override \
    {                                                     \
        return #_eventType;                               \
    }

// 事件类别重写宏
#define EVENT_CLASS_CATEGORY(_category)            \
    int getCategoryFlags() const noexcept override \
    {                                              \
        return _category;                          \
    }

class NAMICA_API EventDispatcher
{
public:
    /**
     * @brief 事件处理函数
     * @details 如果事件处理成功, 返回true表示此事件应该被处理
     */
    template <typename EventT>
    using EventFn = std::function<bool(EventT&)>;

    EventDispatcher(Event& _event) : m_event{_event}
    {
    }
    ~EventDispatcher() = default;

    /**
     * @brief 事件调度
     *
     * @tparam EventT 处理的事件类型
     * @param _eventFn 事件处理函数
     * @return 是否调度成功
     */
    template <typename EventT>
        requires std::is_base_of_v<Event, EventT>
    bool dispatch(EventFn<EventT> _eventFn)
    {
        if (m_event.getType() == EventT::getStaticType())
        {
            m_event.isHandled = _eventFn(*((EventT*)(&m_event)));
            return true;
        }

        return false;
    }

private:
    Event& m_event;
};

}  // namespace Namica