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

#ifndef VOY_BASIC_SINK_H
#define VOY_BASIC_SINK_H

// STL
#include <functional>

// Self
#include "../utils.h"
#include "../dsl/node_tags.h"

using voy::utils::non_copyable;
using voy::dsl::sink_node_tag;

namespace voy {

template <typename F>
class sink: non_copyable {
public:
    using node_category = sink_node_tag;

    explicit sink(F function)
        : m_function{std::move(function)}
    {
    }

    template <typename T>
    void operator() (T&& value) const
    {
        voy_fwd_invoke(m_function, value);
    }

    void init()
    {
    }

    void notify_ended() const
    {
    }

private:
    F m_function;
};


} // namespace voy

#endif // include guard

