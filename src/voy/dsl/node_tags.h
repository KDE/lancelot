/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_DSL_NODE_TAGS_H
#define VOY_DSL_NODE_TAGS_H

// STL
#include <utility>

// Self
#include "../traits.h"

namespace voy::dsl {

// The nodes in the graph are connected only when both the source
// and a sink exist on a path. The node tags are used to detect whether
// something is a source or a sink.

struct source_node_tag {};     // node that only emits messages

struct sink_node_tag {};       // node that only receives messages

struct transformation_node_tag {}; // middle node that defines transformations
                                   // this one does not force establishing
                                   // connection

template <typename Cont>
class continuator_base {
    voy_assert_value_type(Cont);

public:
    continuator_base(Cont&& continuation)
        : m_continuation{std::move(continuation)}
    {
    }

    void init()
    {
        m_continuation.init();
    }

    template <typename T>
    void emit(T&& value) const
    {
        voy_fwd_invoke(m_continuation, value);
    }

    void notify_ended() const
    {
        m_continuation.notify_ended();
    }

protected:
    Cont m_continuation;
};

} // namespace voy::dsl

#endif // include guard

