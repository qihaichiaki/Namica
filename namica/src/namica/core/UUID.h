#pragma once

#include "namica/core/Base.h"
#include <xhash>

namespace Namica
{

class NAMICA_API UUID
{
public:
    UUID();
    UUID(uint64_t _uuid);
    operator uint64_t() const;

private:
    uint64_t m_uuid{0};
};

}  // namespace Namica

namespace std
{
template <>
struct hash<Namica::UUID>
{
    size_t operator()(Namica::UUID const& uuid) const
    {
        return std::hash<uint64_t>()(uuid);
    }
};
}  // namespace std