/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_WRAPPERS_PROCESS_H
#define VOY_WRAPPERS_PROCESS_H

// STL
#include <vector>
#include <functional>

// Self
#include "../utils.h"
#include "../traits.h"
#include "../dsl/node_tags.h"
#include "../engine/asio/process.h"

namespace voy {

using voy::utils::non_copyable;

template <typename ValueHandler, typename... Args>
class process_impl: public non_copyable {
public:
    using node_category = dsl::source_node_tag;

    explicit process_impl(std::tuple<Args...> command,
                          ValueHandler handler = voy::utils::identity{})
        : m_command{std::move(command)}
        , m_handler{std::move(handler)}
    {
    }

    template <typename Cont>
    class node: non_copyable {
        voy_assert_value_type(Cont);

        template <typename F, typename G>
        struct composition {
            voy_assert_value_type(F);
            voy_assert_value_type(G);

            F f;
            G g;

            composition(F f, G g)
                : f{std::move(f)}
                , g{std::move(g)}
            {
            }

            template <typename T>
            decltype(auto) operator() (T&& value) const
            {
                #define EVAL_F_G std::invoke(f, voy_fwd_invoke(g, value))
                if constexpr (std::is_same_v<void, decltype(EVAL_F_G)>) {
                    EVAL_F_G;
                } else {
                    return EVAL_F_G;
                }
                #undef EVAL_F_G
            }

            void init()
            {
                f.init();
            }

            void notify_ended() const
            {
                f.notify_ended();
            }
        };

    public:
        using value_type = decltype(std::declval<ValueHandler>()(std::string{}));

        node(Cont&& cont, ValueHandler&& handler, std::tuple<Args...> command)
            : m_process{
                std::make_unique<voy::engine::asio::process<composition<Cont, ValueHandler>>>(
                    composition<Cont, ValueHandler>{std::move(cont), std::move(handler)},
                    std::move(command)
                )
            }
        {
        }

        void init()
        {
            m_process->init_handler();
        }

    private:
        std::unique_ptr<
            voy::engine::asio::process<composition<Cont, ValueHandler>>> m_process;
    };


    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node<Cont>(
                voy_fwd(cont), std::move(m_handler), std::move(m_command));
    }

private:
    std::tuple<Args...> m_command;
    ValueHandler m_handler;
};

template <typename Cmd, typename... Args>
auto process(Cmd&& cmd, Args&&... args)
{
    return process_impl(
        std::make_tuple(voy_fwd(cmd), voy_fwd(args)...),
        voy::utils::identity{});
}

template <typename Cmd, typename... Args>
auto system_cmd(Cmd&& cmd, Args&&... args)
{
    return process(boost::process::search_path(voy_fwd(cmd)), voy_fwd(args)...);
}

}

#endif // include guard

