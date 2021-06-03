/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

using voy::dsl::continuator_base,
      voy::dsl::source_node_tag;

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
            : base{std::move(cont)}
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
        return node<Cont>(std::move(m_values), voy_fwd(cont));
    }



private:
    std::vector<T> m_values;

};



} // namespace voy

#endif // include guard

