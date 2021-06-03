/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

