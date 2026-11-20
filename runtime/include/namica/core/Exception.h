#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"
#include <exception>

namespace namica
{

class Exception : public std::exception
{
public:
    NAMICA_API explicit Exception(String const& _msg) : m_msg{_msg}
    {
    }

    NAMICA_API const Char* what() const noexcept override
    {
        return m_msg.c_str();
    }

private:
    String m_msg;
};

}  // namespace namica