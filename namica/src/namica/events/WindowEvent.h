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

    WindowResizeEvent(uint32_t _width, uint32_t _height) : m_width(_width), m_height(_height)
    {
    }

    uint32_t getWidth() const noexcept
    {
        return m_width;
    }

    uint32_t getHeight() const noexcept
    {
        return m_height;
    }

    virtual std::string message() const override
    {
        return std::format("窗口大小发生了变化: {0}, {1}", m_width, m_height);
    }

private:
    uint32_t m_width{0};
    uint32_t m_height{0};
};

class NAMICA_API WindowFocusEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowFocus)

    virtual std::string message() const override
    {
        return "窗口被聚焦了";
    }
};

class NAMICA_API WindowLostFocusEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowLostFocus)

    virtual std::string message() const override
    {
        return "窗口失去焦点";
    }
};

class NAMICA_API WindowMovedEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowMoved)

    WindowMovedEvent(int _xpos, int _ypos) : m_xpos{_xpos}, m_ypos{_ypos}
    {
    }

    int getXPos() const noexcept
    {
        return m_xpos;
    }

    int getYPos() const noexcept
    {
        return m_ypos;
    }

    virtual std::string message() const override
    {
        return std::format("窗口被移动了, 新的左上角位置: {0}, {1}", m_xpos, m_ypos);
    }

private:
    int m_xpos{0};
    int m_ypos{0};
};

}  // namespace Namica