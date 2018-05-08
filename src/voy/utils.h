/*
 *   Copyright (C) 2018 Ivan Čukić <ivan.cukic(at)kde.org>
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

#ifndef VOY_UTILS_H
#define VOY_UTILS_H

#include <utility>

// This file contains some things that would be cool to have in
// the standard library. There might be a chance some of these will
// end up in C++20. And a few additional utility functions and macros.

#define voy_fwd(Var) std::forward<decltype(Var)>(Var)
#define voy_fwd_invoke(Fn, Var) std::invoke(Fn, voy_fwd(Var))

namespace voy::utils {

// Private copy constructor and copy assignment ensure classes derived
// from class noncopyable cannot be copied.
namespace disable_adl_ {
    class non_copyable {
    protected:
        constexpr non_copyable() = default;
        ~non_copyable() = default;

        non_copyable(non_copyable&& other) = default;
        non_copyable& operator=(non_copyable&& other) = default;

    private:
        non_copyable(const non_copyable&) = delete;
        non_copyable& operator=(const non_copyable&) = delete;
    };
}

using non_copyable = disable_adl_::non_copyable;


// Identity function

struct identity {
    template <typename T>
    auto operator() (T&& value) const
    {
        return voy_fwd(value);
    }
};


// Overloaded lambdas

template <typename... Fs>
struct overloaded: Fs... {
    using Fs::operator()...;
};

template <typename... Fs>
overloaded(Fs...) -> overloaded<Fs...>;


// Execute code when exiting scope

template <typename F>
struct on_scope_exit {
    on_scope_exit(F f)
        : m_deinit{std::move(f)}
    {
    }

    ~on_scope_exit()
    {
        m_deinit();
    }

    F m_deinit;
};


} // namespace voy::utils

#endif // include guard

