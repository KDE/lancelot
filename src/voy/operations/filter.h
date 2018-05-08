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

#ifndef VOY_OPERATIONS_FILTER_H
#define VOY_OPERATIONS_FILTER_H

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

template <typename Pred>
class filter {
public:
    using node_category = transformation_node_tag;

    explicit filter(Pred predicate)
        : m_predicate{std::move(predicate)}
    {
    }

    template <typename Cont>
    class node: public continuator_base<Cont>, non_copyable {
        using base = continuator_base<Cont>;

    public:
        node(Pred predicate, Cont&& continuation)
            : base{std::move(continuation)}
            , m_predicate{std::move(predicate)}
        {
        }

        template <typename T>
        void operator() (T&& value) const
        {
            if (std::invoke(m_predicate, value)) {
                base::emit(voy_fwd(value));
            }
        }

    private:
        Pred m_predicate;
    };

    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node<Cont>(std::move(m_predicate), voy_fwd(cont));
    }


private:
    Pred m_predicate;
};


template <typename Pred>
auto remove_if(Pred&& pred)
{
    return filter{std::not_fn(pred)};
}


} // namespace voy

#endif // include guard

