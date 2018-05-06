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

#ifndef VOY_DSL_NODE_TAGS_H
#define VOY_DSL_NODE_TAGS_H

namespace voy::dsl {

// The nodes in the graph are connected only when both the source
// and a sink exist on a path. The node tags are used to detect whether
// something is a source or a sink.

struct source_node_tag {};     // node that only emits messages

struct sink_node_tag {};       // node that only receives messages

struct transformation_node_tag {}; // middle node that defines transformations
                                   // this one does not force establishing
                                   // connectiond

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

    void notify_ended() const
    {
        m_continuation.notify_ended();
    }

protected:
    Cont m_continuation;
};

} // namespace voy::dsl

#endif // include guard

