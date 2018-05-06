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

#ifndef VOY_BASIC_VALUES_H
#define VOY_BASIC_VALUES_H

// STL
#include <initializer_list>
#include <vector>
#include <functional>

// Self
#include "../utils.h"
#include "../traits.h"
#include "../dsl/node_tags.h"

namespace voy {

using voy::utils::non_copyable;
using voy::dsl::source_node_tag;
using voy::dsl::continuator_base;

// `values` creates a reactive stream that emits the predefined
// values as soon as the pipeline is initialized

template <typename T>
class values: non_copyable {
    voy_assert_value_type(T);

public:
    using node_category = source_node_tag;

    explicit values(std::initializer_list<T> values)
        : m_values{values}
    {
    }

    template <typename C>
    explicit values(C&& values)
        : m_values{voy_fwd(values)}
    {
    }

    template <typename Cont>
    class node: public continuator_base<Cont>, non_copyable {
        using base = continuator_base<Cont>;

    public:
        node(std::vector<T>&& values, Cont&& cont)
            : continuator_base<Cont>{std::move(cont)}
            , m_values{std::move(values)}
        {
        }

        void init()
        {
            base::init();

            for (auto&& value: m_values) {
                base::emit(std::move(value));
            }

            m_values.clear();

            base::notify_ended();
        }

    private:
        std::vector<T> m_values;
    };


    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node(std::move(m_values), voy_fwd(cont));
    }



private:
    std::vector<T> m_values;

};



} // namespace voy

#endif // include guard

