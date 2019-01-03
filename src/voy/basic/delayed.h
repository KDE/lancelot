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

#ifndef VOY_BASIC_DELAYED_H
#define VOY_BASIC_DELAYED_H

// STL
#include <chrono>
#include <optional>

// Boost
#include <boost/asio.hpp>

// Self
#include "../utils.h"
#include "../traits.h"
#include "../dsl/node_tags.h"
#include "../dsl/node_traits.h"
#include "../engine/asio/service.h"

namespace voy {

using voy::utils::non_copyable;

using voy::dsl::continuator_base,
      voy::dsl::source_node_tag;

namespace detail {

    template <typename T>
    class delayed_impl: non_copyable {
        voy_assert_value_type(T);

    public:
        using node_category = source_node_tag;

        explicit delayed_impl(std::chrono::seconds delay, std::initializer_list<T> c)
            : m_delay{delay}
            , m_values{c}
        {
        }

        template <typename C>
        explicit delayed_impl(std::chrono::seconds delay, C&& c)
            : m_delay{delay}
            , m_values{voy_fwd(c)}
        {
        }

        template <typename Cont>
        class node: continuator_base<Cont>, non_copyable {
            using base = continuator_base<Cont>;

        public:
            using value_type = T;

            node(std::chrono::seconds delay, std::vector<T>&& values, Cont&& cont)
                : base{std::move(cont)}
                , m_delay{delay}
                , m_values{std::move(values)}
            {
            }

            node(node&& other) = default;

            void init()
            {
                base::init();

                m_delay_timer = boost::asio::steady_timer(
                        engine::asio::service::instance(), m_delay);

                m_delay_timer->async_wait([this] (const boost::system::error_code& e) {
                    for (auto&& value: m_values) {
                        base::emit(std::move(value));
                    }

                    m_values.clear();

                    base::notify_ended();
                });
            }

        private:
            std::chrono::seconds m_delay;
            std::vector<T> m_values;
            std::optional<boost::asio::steady_timer> m_delay_timer;
        };

        template <typename Cont>
        auto with_continuation(Cont&& cont) &&
        {
            return node<Cont>(m_delay, std::move(m_values), voy_fwd(cont));
        }

    private:
        std::chrono::seconds m_delay;
        std::vector<T> m_values;
    };

} // namespace detail

template<typename T>
decltype(auto) delayed(std::chrono::seconds delay, T&& value)
{
    return detail::delayed_impl<T>(delay, {voy_fwd(value)});
}

template<typename T>
decltype(auto) delayed_values(std::chrono::seconds delay, T&& values)
{
    return detail::delayed_impl<typename T::value_type>(delay, voy_fwd(values));
}

template<typename T>
decltype(auto) delayed_values(std::chrono::seconds delay, std::initializer_list<T> values)
{
    return detail::delayed_impl<T>(delay, std::move(values));
}

} // namespace voy

#endif // include guard

