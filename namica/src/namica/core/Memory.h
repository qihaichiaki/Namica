#pragma once

#include <memory>

namespace Namica
{

// 便捷友元宏使用
#define FRIEND_PTR_FUNC(ptrName)            \
    template <typename T, typename... Args> \
    friend ptrName<T> create##ptrName(Args&&... args);

#define FRIEND_REF_FUNC FRIEND_PTR_FUNC(Ref)
#define FRIEND_SCOPE_FUNC FRIEND_PTR_FUNC(Scope)

/** Ref */
template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
Ref<T> createRef(Args&&... args)
{
    return Ref<T>{new T{std::forward<Args>(args)...}};
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
    return Scope<T>{new T{std::forward<Args>(args)...}};
}

}  // namespace Namica