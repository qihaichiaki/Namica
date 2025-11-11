#pragma once

#include "namica/events/Event.h"
#include <format>

namespace Namica
{

class NAMICA_API WindowCloseEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowClose)

    virtual std::string message() const override
    {
        return "窗口被关闭了";
    }
};

class NAMICA_API WindowResizeEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowResize)

    WindowResizeEvent(int _width, int _height) : m_width(_width), m_height(_height)
    {
    }

    int getWidth() const noexcept
    {
        return m_width;
    }

    int getHeight() const noexcept
    {
        return m_height;
    }

    virtual std::string message() const override
    {
        return std::format("窗口大小发生了变化: {0}, {1}", m_width, m_height);
    }

private:
    int m_width{0};
    int m_height{0};
};

class WindowFocusEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowFocus)

    virtual std::string message() const override
    {
        return "窗口被聚焦了";
    }
};

class WindowLostFocusEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowLostFocus)

    virtual std::string message() const override
    {
        return "窗口失去焦点";
    }
};

class WindowMovedEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowMoved)

    virtual std::string message() const override
    {
        return "窗口被移动了";
    }
};

}  // namespace Namica