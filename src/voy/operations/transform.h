/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_OPERATIONS_TRANSFORM_H
#define VOY_OPERATIONS_TRANSFORM_H

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

template <typename Traf>
class transform {
public:
    using node_category = transformation_node_tag;

    explicit transform(Traf transformation)
        : m_transformation(std::move(transformation))
    {
    }

    template <typename Cont>
    class node: public continuator_base<Cont>, non_copyable {
        using base = continuator_base<Cont>;

    public:
        node(Traf transformation, Cont&& continuation)
            : base{std::move(continuation)}
            , m_transformation{std::move(transformation)}
        {
        }

        template <typename T>
        void operator() (T&& value) const
        {
            base::emit(voy_fwd_invoke(m_transformation, value));
        }

    private:
        Traf m_transformation;
    };

    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node<Cont>(std::move(m_transformation), voy_fwd(cont));
    }

    template < typename Cont
             , voy_require(std::is_copy_constructible_v<Traf>)
             >
    auto with_continuation(Cont&& cont) const
    {
        return node(m_transformation, voy_fwd(cont));
    }

private:
    Traf m_transformation;
};

template <typename Type>
inline auto cast_to = transform {
        [] (auto&& value) {
            return static_cast<Type>(voy_fwd(value));
        }
    };

} // namespace voy

#endif // include guard

