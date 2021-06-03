/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_BASIC_SINK_H
#define VOY_BASIC_SINK_H

// STL
#include <functional>

// Self
#include "../utils.h"
#include "../dsl/node_tags.h"

namespace voy {

using voy::utils::non_copyable;
using voy::dsl::sink_node_tag;

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

template <>
class sink<void>: non_copyable {
public:
    using node_category = sink_node_tag;

    explicit sink()
    {
    }

    template <typename T>
    void operator() (T&& value) const
    {
    }

    void init()
    {
    }

    void notify_ended() const
    {
    }
};


} // namespace voy

#endif // include guard

