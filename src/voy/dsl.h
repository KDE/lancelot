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

using node_traits::is_connection_expr,
      node_traits::is_node,
      node_traits::is_source,
      node_traits::is_sink,
      node_traits::node_category,
      node_traits::with_continuation_memfn;

// We need to be able to store the connected graph paths (pipelines)
// inside of ordinary classes, so we will return a type-erased pipeline
// to the caller once the path is complete
class pipeline {
public:
    virtual ~pipeline() {}
    virtual void init() = 0;

    using ptr = std::unique_ptr<pipeline>;
};

namespace detail {
    // The actual type of a pipeline
    template <typename T>
    struct pipeline_impl: pipeline {
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
    struct node_wrapper: utils::non_copyable {
        voy_assert_value_type(Node);

        node_wrapper(Node node)
            : node{std::move(node)}
        {
        }

        node_wrapper(node_wrapper&& other) = delete;
        void operator=(node_wrapper&&) = delete;

        void init()
        {
            node.init();
        }

        Node node;
    };


    template <typename Node>
    auto make_node_wrapper(Node&& node)
    {
        voy_assert_value_type(Node);
        return node_wrapper<Node>{std::move(node)};
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
    pipeline::ptr connect_all(std::tuple<Args...> items,
                              std::index_sequence<Idx...>)
    {
        return std::make_unique<
                decltype(pipeline_impl((... << make_node_wrapper(std::get<Idx>(std::move(items))))))
            > ((... << make_node_wrapper(std::get<Idx>(std::move(items)))));
    }



    // The connection_expr class represents one pipe operation
    // in the AST where the left and right arguments can be either
    // compound expressions themselves, or single nodes
    template <typename NodeCategory, typename LeftGraph, typename RightGraph>
    struct connection_expr {
        voy_assert_value_type(LeftGraph);
        voy_assert_value_type(RightGraph);

        LeftGraph left;
        RightGraph right;

        // An expression is also a graph node
        using node_category = NodeCategory;
        // ... but we still need to be able to differentiate it from normal nodes
        using connection_expr_tag = node_category;

        // Generates a tuple of all nodes in an expression from right to left,
        // that is, from the sink to the source
        auto collect_graph_nodes()
        {
            auto collect_left_graph_nodes = [this]
            {
                if constexpr (is_connection_expr<LeftGraph>) {
                    return left.collect_graph_nodes();
                } else {
                    return std::make_tuple(std::move(left));
                }
            };

            auto collect_right_graph_nodes = [this]
            {
                if constexpr (is_connection_expr<RightGraph>) {
                    return right.collect_graph_nodes();
                } else {
                    return std::make_tuple(std::move(right));
                }
            };

            return std::tuple_cat(collect_right_graph_nodes(),
                                  collect_left_graph_nodes());
        }

        connection_expr(LeftGraph left, RightGraph right)
            : left{std::move(left)}
            , right{std::move(right)}
        {
        }

        auto evaluate()
        {
            if constexpr (std::is_same_v<void, node_category>) {
                auto sink_to_source_items = collect_graph_nodes();

                auto result = connect_all(std::move(sink_to_source_items),
                        std::make_index_sequence<
                            std::tuple_size_v<
                                std::decay_t<decltype(sink_to_source_items)>
                            >
                        >());

                // async call result->init()

                return result;

            } else {
                voy_fail(node_category, "'evaluate' can only be called on a complete path");
            }
        }
    };


    template < typename NodeCategory
             , typename LeftGraph
             , typename RightGraph
             >
    auto make_connection_expr(LeftGraph&& left, RightGraph&& right)
    {
        return connection_expr < NodeCategory
                               , traits::remove_cvref_t<LeftGraph>
                               , traits::remove_cvref_t<RightGraph>
                               > { voy_fwd(left), voy_fwd(right) };
    }


    template < typename Left
             , typename Right
             , typename LeftVal  = traits::remove_cvref_t<Left>
             , typename RightVal = traits::remove_cvref_t<Right>
             >
    decltype(auto) connect_streams(Left&& left, Right&& right)
    {
        static_assert(is_node<Left>, "The left needs to be a node");
        static_assert(is_node<Right>, "The right needs to be a node");
        static_assert(
                is_connection_expr<Left> ||
                is_detected_v<with_continuation_memfn, Left>,
                "The left node needs to be a connection expression, or to have with_continuation member function");

        #define MAKE(Type)                                                     \
            make_connection_expr<Type, LeftVal, RightVal>(std::move(left),     \
                                                          std::move(right))

        if constexpr (!is_source<Left> && !is_sink<Right>) {

            return MAKE(transformation_node_tag);

        } else if constexpr (is_source<Left> && !is_sink<Right>) {

            return MAKE(node_category<Left>);

        } else if constexpr (!is_source<Left> && is_sink<Right>) {

            return MAKE(node_category<Right>);

        } else {

            // If we have both a sink and a source, we can connect the
            // nodes in this path of the graph
            return MAKE(void).evaluate();

        }

        #undef MAKE
    }
} // namespace detail


template < typename Left
         , typename Right
         , typename LeftVal  = traits::remove_cvref_t<Left>
         , typename RightVal = traits::remove_cvref_t<Right>
         , voy_require(is_node<Left> && is_node<Right>)
         , voy_require((
               is_connection_expr<Left> ||
               is_detected_v<with_continuation_memfn, Left>
           ))
         >
decltype(auto) operator| (Left&& left, Right&& right)
{
    return detail::connect_streams(voy_fwd(left), voy_fwd(right));
}


} // namespace voy::dsl

#endif // include guard

