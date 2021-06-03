/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_OPERATIONS_MERGE_H
#define VOY_OPERATIONS_MERGE_H

// STL
#include <functional>
#include <tuple>

// Self
#include "../utils.h"
#include "../dsl.h"

namespace voy {

using voy::utils::non_copyable;

using voy::dsl::source_node_tag;

/**
 * merge is a class that combines multiple streams into one.
 *
 * Class design:
 *  - The class behaves like a source stream -- when the source streams
 *    are mergged, the rest of the program will see the merged stream
 *    as a single source of values
 *  - Internally, it needs to connect to the source streams in order to
 *    be able to pass the values on which means that it creates a
 *    sink for each of the source streams and connects to them
 *
 */
template <typename... Sources>
class merge {
public:
    using node_category = source_node_tag;

    explicit merge(Sources... sources)
        : m_sources{std::make_tuple(std::move(sources)...)}
    {
    }

    template <typename Cont>
    class node: non_copyable {
    private:

        // Connections take ownership of the connected objects.
        // We can not allow this to happen as the merged stream
        // connects to several streams at once. Instead, we
        // will give just a dummy reference for the connections
        // to take ownership of. Now, both the node and the
        // references can be moved to new instances, so this
        // is a bit more complicated than it seems.

        struct impl {
            struct internal_connection_base;
            std::array<std::unique_ptr<internal_connection_base>, sizeof...(Sources)> sources;

            mutable int sources_active;
            Cont continuation;

            // When connecting the sources, we want to type-erase the
            // connection and just keep a pointer to it to extend
            // its lifetime
            struct internal_connection_base { //_
            public:
                virtual ~internal_connection_base() {}
            };

            template <typename T>
            struct internal_connection: internal_connection_base {
            public:
                internal_connection(T connection)
                    : connection{std::move(connection)}
                {
                    // utils::print_types<T>();
                }

                T connection;
            };

            struct ref {
                using node_category = sink_node_tag;

                ref(impl* _this)
                    : m_this{_this}
                {
                }

                template <typename T>
                void operator() (T&& value) const
                {
                    m_this->operator()(voy_fwd(value));
                }

                void init()
                {
                    // TODO: Should we react to init?
                }

                void notify_ended() const
                {
                    m_this->notify_ended();
                }

                impl* m_this;
            };

            // Creates and instance of the type-erased connection
            template <typename Source>
            std::unique_ptr<internal_connection_base> create_connection(
                    size_t index,
                    Source&& source)
            {
                return
                    std::make_unique<
                        internal_connection<
                            decltype(dsl::detail::connect_streams(
                                std::move(source),
                                ref(this)))
                        >
                    >
                        (
                            dsl::detail::connect_streams(
                                std::move(source),
                                ref(this))
                        )
                ;
            }

            template <size_t... Idx>
            decltype(sources) create_connections(std::tuple<Sources...> sources,
                                                 std::index_sequence<Idx...>)
            {
                return { create_connection(Idx, std::get<Idx>(std::move(sources)))... };
            } //^

            impl(std::tuple<Sources...> sources, Cont continuation)
                : sources{create_connections(std::move(sources), std::index_sequence_for<Sources...>())}
                , sources_active{sizeof...(Sources)}
                , continuation{std::move(continuation)}
            {
            }

            void init()
            {
            }

            void notify_ended() const
            {
                if (0 == --sources_active) {
                    continuation.notify_ended();
                }
            }

            template <typename T>
            void operator() (T&& value)
            {
                voy_fwd_invoke(continuation, value);
            }

            ~impl() = default;
            impl(const impl&) = delete;
            impl(impl&&) = delete;
            impl& operator=(impl) = delete;
        };
        std::unique_ptr<impl> d;

    public:
        node(std::tuple<Sources...> sources, Cont&& continuation)
            : d{std::make_unique<impl>(std::move(sources), std::move(continuation))}
        {
        }

        ~node() = default;
        node(node&& other) noexcept = default;
        node& operator=(node other) = delete;

        void init()
        {
            d->init();
        }

        template <typename T>
        void operator() (T&& value) const
        {
            voy_fwd_invoke(d->m_continuation, value);
        }

        void notify_ended() const
        {
            d->notify_ended();
        }

    };

    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node<Cont>(std::move(m_sources), voy_fwd(cont));
    }

private:
    std::tuple<Sources...> m_sources;
};

} // namespace voy

#endif // include guard

