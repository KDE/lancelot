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

#ifndef VOY_DSL_MULTIPROCESS_H
#define VOY_DSL_MULTIPROCESS_H

#include "../operations/identity.h"

#include "dsl.h"

namespace voy::dsl {

namespace detail {

    template <typename... Pipelines>
    struct pipeline_container {
        template <typename LeftTuple, typename RightTuple>
        pipeline_container(LeftTuple&& left, RightTuple&& right)
            : m_pipelines{std::tuple_cat(voy_fwd(left), voy_fwd(right))}
        {
        }

        template <typename Tuple>
        pipeline_container(Tuple&& tuple)
            : m_pipelines{std::move(tuple)}
        {
        }

        pipeline_container()
        {
        }

        template <typename NewPipeline>
        auto operator|| (NewPipeline&& new_pipeline) &&
        {
            if constexpr (std::is_same_v<NewPipeline, voy::dsl::pipeline::ptr>) {
                return pipeline_container<Pipelines..., NewPipeline>(
                        std::move(m_pipelines), std::make_tuple(voy_fwd(new_pipeline)));

            } else {
                return pipeline_container<Pipelines...>(
                        std::move(m_pipelines));

            }
        }

        std::tuple<Pipelines...> m_pipelines;
    };

    inline auto multiprocess_pipeline()
    {
        return pipeline_container<>{};
    }

    template < typename Left
             >
    voy_concept supports_double_pipeline(Left* = nullptr)
    {
        if constexpr (std::is_same_v<Left, pipeline::ptr>) {
            return true;

        } else if constexpr (node_traits::is_connection_expr<Left>) {
            return true;

        } else if constexpr (node_traits::is_node<Left>) {
            return true;

        } else {
            return false;
        }
    }


} // namespace detail

template < typename Left
         , typename Right
         , voy_require(
               detail::supports_double_pipeline<Left>()
           )
         >
auto operator||(Left&& left, Right&& right)
{
    return voy::dsl::detail::multiprocess_pipeline()
           || voy_fwd(left)
           || voy_fwd(right);
}

} // namespace voy::dsl


#define voy_declare_bridge_out(BridgeName)                                     \
    auto BridgeName##_send()                                                   \
    {                                                                          \
        return voy::zmq::publisher<>(                                          \
            std::string("ipc:///tmp/voy-zmq-bridge-" #BridgeName "-ivan"));    \
    }                                                                          \
                                                                               \
    auto BridgeName##_receive()                                                \
    {                                                                          \
        return voy::identity<>();                                              \
    }

#define voy_declare_bridge_in(BridgeName)                                      \
    auto BridgeName##_send()                                                   \
    {                                                                          \
        return voy::identity<>();                                              \
    }                                                                          \
                                                                               \
    auto BridgeName##_receive()                                                \
    {                                                                          \
        return voy::zmq::subscriber<>(                                         \
            std::string("ipc:///tmp/voy-zmq-bridge-" #BridgeName "-ivan"));    \
    }

#define voy_declare_bridge_ignored(BridgeName)                                 \
    auto BridgeName##_send()                                                   \
    {                                                                          \
        return voy::identity<>();                                              \
    }                                                                          \
                                                                               \
    auto BridgeName##_receive()                                                \
    {                                                                          \
        return voy::identity<>();                                              \
    }

#define voy_bridge(BridgeName) BridgeName##_send() || BridgeName##_receive()
#define voy_multiprocess voy::dsl::multiprocess_pipeline() ||

#endif // include guard

