/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_NODE_TRAITS_H
#define VOY_NODE_TRAITS_H

// STL
#include <utility>

#include "node_tags.h"

#include "../traits.h"
#include "../utils.h"
#include "../basic/sink.h"

namespace voy::dsl::node_traits {

using namespace voy::traits;

// Returns the category of the node. It can also be used to detect whether
// a class contains the node category specification
template <typename Node>
using node_category = typename remove_cvref_t<Node>::node_category;


// Used to detect whther the class contains a with_continuation function
// for rvalues
template <typename Node>
using with_continuation_memfn = decltype(
        std::declval<Node&&>().with_continuation(voy::sink<void>{}));

template <typename Node>
voy_concept has_with_continuation = is_detected_v<with_continuation_memfn, Node>;


// Returns whether the node has the specified tag or not
template <typename Tag, typename Node>
voy_concept is_tagged_with = std::is_same_v<Tag, node_category<Node>>;


// The meta-function to check whether a class is a node is easier to implement
// using the constexpr-if because of the branching.
namespace detail {
    template < typename Node
             , typename DetectedCategory = detected_t<node_category, Node>
             >
    voy_concept is_node()
    {
        if constexpr (!is_detected_v<node_category, Node>) {
            // voy_fail(Node, "No category specified for the node");
            return false;

        } else if constexpr (std::is_same_v<void, DetectedCategory>) {
            // voy_fail(Node, "Category is void, which means we already have a complete graph path");
            return false;

        } else {
            return true;
        }
    }
}

// Wrapper for detail::is_node to make it look like a normal meta-function
template <typename Node>
voy_concept is_node = detail::is_node<Node>();


// Checks whether the node is a source node
template <typename Node>
voy_concept is_source = is_tagged_with<source_node_tag, Node>;


// Checks whether the node is a source node
template <typename Node>
voy_concept is_sink = is_tagged_with<sink_node_tag, Node>;


// To be used with the detection idiom to check whether a type is
// a connection_expr or a normal node
template <typename Graph>
using connection_expr_tag_definition = typename Graph::connection_expr_tag;

template <typename Node>
voy_concept is_connection_expr =
    is_detected_v<connection_expr_tag_definition, Node>;



} // namespace voy::dsl::node_traits

#endif // include guard

