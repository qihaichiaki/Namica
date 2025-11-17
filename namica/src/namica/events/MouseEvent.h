#pragma once

#include "namica/events/Event.h"
#include "namica/core/InputCodes.h"
#include <format>

namespace Namica
{

class NAMICA_API MouseButtonEvent : public Event
{
public:
    KeyCode getMouseButton() const noexcept
    {
        return m_button;
    }

protected:
    MouseButtonEvent(KeyCode _button) : m_button{_button}
    {
    }

    KeyCode m_button;
};

class NAMICA_API MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(KeyCode _button) : MouseButtonEvent{_button}
    {
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)

    virtual std::string message() const override
    {
        return std::format("鼠标键:{0} 被按下", static_cast<uint16_t>(m_button));
    }
};

class NAMICA_API MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent(KeyCode _button) : MouseButtonEvent{_button}
    {
    }

    EVENT_CLASS_TYPE(MouseButtonReleased)

    virtual std::string message() const override
    {
        return std::format("鼠标键:{0} 被释放", static_cast<uint16_t>(m_button));
    }
};

class NAMICA_API MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(float x_pos, float y_pos) : m_xPos{x_pos}, m_yPos{y_pos}
    {
    }

    EVENT_CLASS_TYPE(MouseMoved)

    float getPosX() const noexcept
    {
        return m_xPos;
    }
    float getPosY() const noexcept
    {
        return m_yPos;
    }

    virtual std::string message() const override
    {
        return std::format("鼠标键指针移动: ({0}, {1})", m_xPos, m_yPos);
    }

private:
    float m_xPos, m_yPos;
};

class NAMICA_API MouseScrolledEvent : public Event
{
public:
    MouseScrolledEvent(float x_offset, float y_offset) : m_xOffset{x_offset}, m_yOffset{y_offset}
    {
    }

    EVENT_CLASS_TYPE(MouseScrolled)

    float getOffsetX() const noexcept
    {
        return m_xOffset;
    }

    float getOffsetY() const noexcept
    {
        return m_yOffset;
    }

    virtual std::string message() const override
    {
        return std::format("鼠标键滚轮移动: ({0}, {1})", m_xOffset, m_yOffset);
    }

private:
    float m_xOffset, m_yOffset;
};
}  // namespace Namica