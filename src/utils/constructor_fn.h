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

