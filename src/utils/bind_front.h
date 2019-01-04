/*
 *   Copyright (C) 2019 Ivan Čukić <ivan.cukic(at)kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.
 *   If not, see <http://www.gnu.org/licenses/>.
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

