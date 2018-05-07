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

#ifndef VOY_DSL_H
#define VOY_DSL_H

// STL
#include <memory>
#include <tuple>

// Self
#include "utils.h"
#include "traits.h"
#include "dsl/node_tags.h"
#include "dsl/node_traits.h"

namespace voy::dsl {

using namespace voy::traits;
using namespace node_traits;


// We need to be able to store the connected graph paths (pipelines)
// inside of ordinary classes, so we will return a type-erased pipeline
// to the caller once the path is complete
class pipeline {
public:
    using ptr = std::unique_ptr<pipeline>;

    virtual ~pipeline() {}
    virtual void init() = 0;
};

namespace detail {
    // The actual type of a pipeline
    template <typename T>
    class pipeline_impl: public pipeline {
    public:
        explicit pipeline_impl(T&& content) noexcept
            : node{std::move(content.node)}
        {
        }

        void init() override
        {
            node.init();
        }

        T node;
    };



    // In order to use the fold expressions, we need to provide an operator
    // on which to fold. We can create a wrapper type with operator<<
    // defined for it
    template <typename Node>
    class node_wrapper: utils::non_copyable {
        voy_assert_value_type(Node);

    public:
        node_wrapper(Node&& node)
            : node{std::move(node)}
        {
        }

        node_wrapper(node_wrapper&&) = delete;
        node_wrapper& operator=(node_wrapper&&) = delete;

        void init()
        {
            node.init();
        }

        Node node;
    };

    template <typename Node>
    auto make_node_wrapper(Node&& node)
    {
        return node_wrapper<Node>(voy_fwd(node));
    }

    template <typename Left, typename Right>
    decltype(auto) operator<< (node_wrapper<Left>&& receiver,
                               node_wrapper<Right>&& sender)
    {
        voy_assert_value_type(Left);
        voy_assert_value_type(Right);

        return make_node_wrapper(
                std::move(sender.node).with_continuation(std::move(receiver.node))
            );
    }

    // Goes through all the items in a tuple, and connects one by one
    template <typename... Args, size_t... Idx>
    pipeline::ptr connect_all(std::tuple<Args...>&& items,
                              std::index_sequence<Idx...>)
    {
        return std::make_unique<
                decltype(pipeline_impl((... << make_node_wrapper(std::get<Idx>(std::move(items))))))
            > ((... << make_node_wrapper(std::get<Idx>(std::move(items)))));
    }

} // namespace detail


} // namespace voy::dsl

#endif // include guard

