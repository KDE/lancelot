/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_DSL_MULTIPROCESS_H
#define VOY_DSL_MULTIPROCESS_H

#include "../operations/identity.h"

#include "../dsl.h"

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
        using namespace voy::zmq;                                              \
        return publisher<>(ipc(#BridgeName));                                  \
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
        using namespace voy::zmq;                                              \
        return subscriber<>(ipc(#BridgeName));                                 \
    }

#define voy_declare_bridge_spread(BridgeName)                                  \
    auto BridgeName##_send()                                                   \
    {                                                                          \
        using namespace voy::zmq;                                              \
        return publisher<policy::spread>(ipc(#BridgeName));                    \
    }                                                                          \
                                                                               \
    auto BridgeName##_receive()                                                \
    {                                                                          \
        return voy::identity<>();                                              \
    }

#define voy_declare_bridge_accept(BridgeName)                                  \
    auto BridgeName##_send()                                                   \
    {                                                                          \
        return voy::identity<>();                                              \
    }                                                                          \
                                                                               \
    auto BridgeName##_receive()                                                \
    {                                                                          \
        using namespace voy::zmq;                                              \
        return subscriber<policy::spread>(ipc(#BridgeName));                   \
    }

#define voy_declare_bridge_post(BridgeName)                                    \
    auto BridgeName##_send()                                                   \
    {                                                                          \
        using namespace voy::zmq;                                              \
        return publisher<policy::collect>(ipc(#BridgeName));                   \
    }                                                                          \
                                                                               \
    auto BridgeName##_receive()                                                \
    {                                                                          \
        return voy::identity<>();                                              \
    }

#define voy_declare_bridge_collect(BridgeName)                                 \
    auto BridgeName##_send()                                                   \
    {                                                                          \
        return voy::identity<>();                                              \
    }                                                                          \
                                                                               \
    auto BridgeName##_receive()                                                \
    {                                                                          \
        using namespace voy::zmq;                                              \
        return subscriber<policy::collect>(ipc(#BridgeName));                  \
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

