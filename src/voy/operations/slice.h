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

#ifndef VOY_OPERATIONS_SLICE_H
#define VOY_OPERATIONS_SLICE_H

// STL
#include <functional>

// Self
#include "../utils.h"
#include "../traits.h"
#include "../dsl/node_tags.h"

namespace voy {

using voy::utils::non_copyable;

using voy::dsl::continuator_base,
      voy::dsl::transformation_node_tag;

template <typename = void>
class slice_impl {
public:
    using node_category = transformation_node_tag;

    explicit slice_impl(size_t drop, size_t take)
        : m_drop{drop}
        , m_take{take}
    {
    }

    template <typename Cont>
    class node: public continuator_base<Cont>, non_copyable {
        using base = continuator_base<Cont>;

    public:
        node(size_t drop, size_t take, Cont&& continuation)
            : base(std::move(continuation))
            , m_drop(drop)
            , m_take(take)
        {
        }

        template <typename T>
        void operator() (T&& value) const
        {
            //  0 1 2 3 4 5 6 7 8 9    drop 3 take 5
            //        [         )
            //        3        3+5
            utils::on_scope_exit increment_count = [&] {
                if (m_current_count < m_drop + m_take) {
                    m_current_count++;
                }
            };

            if (m_current_count < m_drop) return;
            if (m_take != 0 && m_current_count >= m_drop + m_take) return;

            base::emit(voy_fwd(value));

            if (m_current_count == m_drop + m_take - 1) {
                this->m_continuation.notify_ended();
            }
        }

    private:
        size_t m_drop;
        size_t m_take;

        // no need for locking, nodes are not shared across threads
        mutable size_t m_current_count = 0;
    };

    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node<Cont>(m_drop, m_take, voy_fwd(cont));
    }

private:
    size_t m_drop;
    size_t m_take;
};

inline
auto drop(size_t drop_count)
{
    return slice_impl(drop_count, 0);
}

inline
auto take(size_t take_count)
{
    return slice_impl(0, take_count);
}

inline
auto slice(size_t drop_count, size_t take_count)
{
    return slice_impl(drop_count, take_count);
}


} // namespace voy

#endif // include guard

