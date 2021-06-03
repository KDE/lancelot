/*
    SPDX-FileCopyrightText: 2019 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef UTILS_CONSTRUCTOR_FN_H
#define UTILS_CONSTRUCTOR_FN_H

#include <type_traits>

template <typename R>
struct construct {
    template <typename... Args>
    R operator() (Args&&... args) const
        noexcept(std::is_nothrow_constructible_v<R, Args&&...>)
    {
        return R(std::forward<Args>(args)...);
    }
};

template <typename R>
struct initialize {
    template <typename... Args>
    R operator() (Args&&... args) const
        noexcept(std::is_nothrow_constructible_v<R, Args&&...>)
    {
        return R{std::forward<Args>(args)...};
    }
};

#endif // include guard

