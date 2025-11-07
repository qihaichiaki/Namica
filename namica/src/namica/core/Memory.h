#pragma once

#include <memory>

namespace Namica
{

/** Ref */
template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
Ref<T> createRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

/** WeakRef */
template <typename T>
using WeakRef = std::weak_ptr<T>;

/** Scope */
template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
Scope<T> createScope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

}  // namespace Namica