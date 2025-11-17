#pragma once

#include "namica/events/Event.h"
#include "namica/core/InputCodes.h"
#include <format>

namespace Namica
{

class NAMICA_API KeyEvent : public Event
{
public:
    KeyCode getKeyCode() const noexcept
    {
        return m_keyCode;
    }

protected:
    KeyEvent(KeyCode _keyCode) : m_keyCode(_keyCode)
    {
    }

    KeyCode m_keyCode;
};

class NAMICA_API KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(KeyCode _keyCode, int _repeatCount)
        : KeyEvent(_keyCode), m_repeatCount(_repeatCount)
    {
    }

    EVENT_CLASS_TYPE(KeyPressed)

    int getRepeatCount() const noexcept
    {
        return m_repeatCount;
    }

    virtual std::string message() const override
    {
        return std::format(
            "按键:{0} 被按下, repeatCount:{1}", static_cast<uint16_t>(m_keyCode), m_repeatCount);
    }

private:
    int m_repeatCount;
};

class NAMICA_API KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(KeyCode _keyCode) : KeyEvent(_keyCode)
    {
    }

    EVENT_CLASS_TYPE(KeyReleased)

    virtual std::string message() const override
    {
        return std::format("按键:{0} 被释放", static_cast<uint16_t>(m_keyCode));
    }
};

class NAMICA_API KeyTypedEvent : public Event
{
public:
    KeyTypedEvent(uint32_t _codepoint) : m_codepoint(_codepoint)
    {
    }

    EVENT_CLASS_TYPE(KeyTyped)

    uint32_t getCodepoint() const noexcept
    {
        return m_codepoint;
    }

    virtual std::string message() const override
    {
        return std::format("输入字符: {0}", m_codepoint);
    }

private:
    uint32_t m_codepoint;
};

}  // namespace Namica