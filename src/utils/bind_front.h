/*
    SPDX-FileCopyrightText: 2019 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef UTILS_FRONT_BIND_H
#define UTILS_FRONT_BIND_H

namespace std_ex {

// bind_front is based on http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0356r3.html

template <typename T> struct decay_unwrap;

template <typename T> struct decay_unwrap<std::reference_wrapper<T>> {
    using type = T &;
};

template <typename T>
struct decay_unwrap
    : std::conditional_t<!std::is_same<std::decay_t<T>, T>::value,
                         decay_unwrap<std::decay_t<T>>, std::decay<T>> {
};

template <typename T> using decay_unwrap_t = typename decay_unwrap<T>::type;

template <typename Func, typename BoundArgsTuple, typename... CallArgs>
decltype(auto) bind_front_caller(Func &&func, BoundArgsTuple &&boundArgsTuple,
                                 CallArgs &&... callArgs)
{
    return std::apply(
        [&func, &callArgs...](auto &&... boundArgs) -> decltype(auto) {
            return std::invoke(std::forward<Func>(func),
                               std::forward<decltype(boundArgs)>(boundArgs)...,
                               std::forward<CallArgs>(callArgs)...);
        },
        std::forward<BoundArgsTuple>(boundArgsTuple));
}

template <typename Func, typename... BoundArgs> class bind_front_t {
public:
    template <
        typename F, typename... BA,
        std::enable_if_t<!(sizeof...(BA) == 0
                           && std::is_base_of_v<bind_front_t, std::decay_t<F>>),
                         bool> = true>
    explicit bind_front_t(F &&f, BA &&... ba)
        : func(std::forward<F>(f)), boundArgs(std::forward<BA>(ba)...)
    {
    }

    template <typename... CallArgs>
        auto operator()(CallArgs &&... callArgs)
        & noexcept(
              std::is_nothrow_invocable_v<Func &, BoundArgs &..., CallArgs...>)
              -> std::invoke_result_t<Func &, BoundArgs &..., CallArgs...>
    {
        return bind_front_caller(func, boundArgs,
                                 std::forward<CallArgs>(callArgs)...);
    }

    template <typename... CallArgs>
    auto operator()(CallArgs &&... callArgs) const& noexcept(
        std::is_nothrow_invocable_v<const Func&, const BoundArgs&...,
                                    CallArgs...>)
        -> std::invoke_result_t<const Func&, const BoundArgs&..., CallArgs...>
    {
        return bind_front_caller(func, boundArgs,
                                 std::forward<CallArgs>(callArgs)...);
    }

    template <typename... CallArgs>
        auto operator()(CallArgs &&... callArgs)
        && noexcept(
               std::is_nothrow_invocable_v<Func, BoundArgs..., CallArgs...>)
               -> std::invoke_result_t<Func, BoundArgs..., CallArgs...>
    {
        return bind_front_caller(std::move(func), std::move(boundArgs),
                                 std::forward<CallArgs>(callArgs)...);
    }

    template <typename... CallArgs>
    auto operator()(CallArgs &&... callArgs) const&& noexcept(
        std::is_nothrow_invocable_v<const Func, const BoundArgs...,
                                    CallArgs...>)
        -> std::invoke_result_t<const Func, const BoundArgs..., CallArgs...>
    {
        return bind_front_caller(std::move(func), std::move(boundArgs),
                                 std::forward<CallArgs>(callArgs)...);
    }

private:
    Func func;
    std::tuple<BoundArgs...> boundArgs;
};

template <typename Func, typename... BoundArgs>
auto bind_front(Func &&func, BoundArgs &&... boundArgs)
{
    return bind_front_t<std::decay_t<Func>, decay_unwrap_t<BoundArgs>...>{
        std::forward<Func>(func), std::forward<BoundArgs>(boundArgs)...
    };
}

} // namespace std_ex

#endif // include guard

