/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_OPERATIONS_IDENTITY_H
#define VOY_OPERATIONS_IDENTITY_H

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
class identity {
public:
    using node_category = transformation_node_tag;

    explicit identity()
    {
    }

    template <typename Cont>
    class node: public continuator_base<Cont>, non_copyable {
        using base = continuator_base<Cont>;

    public:
        node(Cont&& continuation)
            : base{std::move(continuation)}
        {
        }

        template <typename T>
        void operator() (T&& value) const
        {
            base::emit(voy_fwd(value));
        }
    };

    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node<Cont>(voy_fwd(cont));
    }
};

} // namespace voy

#endif // include guard

