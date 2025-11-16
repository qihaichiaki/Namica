#pragma once

#include <memory>

namespace Namica
{

// 便捷友元宏使用
#define FRIEND_PTR_FUNC(ptrName)            \
    template <typename T, typename... Args> \
    friend ptrName<T> create##ptrName(Args&&... _args);

#define FRIEND_REF_FUNC FRIEND_PTR_FUNC(Ref)
#define FRIEND_SCOPE_FUNC FRIEND_PTR_FUNC(Scope)

/** Ref */
template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
Ref<T> createRef(Args&&... _args)
{
    return Ref<T>{new T{std::forward<Args>(_args)...}};
}

template <typename D, typename B>
    requires std::is_base_of_v<B, D>
Ref<D> refCast(Ref<B> const& _baseRef)
{
    return std::static_pointer_cast<D>(_baseRef);
}

/** WeakRef */
template <typename T>
using WeakRef = std::weak_ptr<T>;

/** Scope */
template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
Scope<T> createScope(Args&&... _args)
{
    return Scope<T>{new T{std::forward<Args>(_args)...}};
}

}  // namespace Namica