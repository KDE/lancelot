/*
 *   Copyright (C) 2019 Ivan Čukić <ivan.cukic(at)kde.org>
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

#ifndef VOY_DEBOUNCE_H
#define VOY_DEBOUNCE_H

// STL
#include <chrono>
#include <optional>
#include <iostream>

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
    class debounce_impl: non_copyable {
        voy_assert_value_type(T);

    public:
        using node_category = transformation_node_tag;

        explicit debounce_impl(std::chrono::milliseconds delay)
            : m_delay{delay}
        {
        }

        template <typename Cont>
        class node: public continuator_base<Cont>, non_copyable {
            using base = continuator_base<Cont>;

        public:
            node(std::chrono::milliseconds delay, Cont&& cont)
                : base{std::move(cont)}
                , m_delay{delay}
            {
            }

            // node(node&& other) = default;
            node(node&& other) noexcept
                : base(std::move(other))
                , m_delay{other.m_delay}
            {
            }

            void init()
            {
                base::init();

                m_delay_timer = boost::asio::steady_timer(
                        engine::asio::service::instance());
            }

            void operator() (T&& value) const
            {
                m_last_value = std::move(value);
                using namespace std::literals::chrono_literals;

                m_delay_timer->expires_after(m_delay);
                m_delay_timer->async_wait([this] (const boost::system::error_code& error) {
                    if (error) return;
                    base::emit(std::move(m_last_value));
                });
            }

        private:
            std::chrono::milliseconds m_delay;
            mutable std::optional<boost::asio::steady_timer> m_delay_timer;
            mutable T m_last_value;
        };

        template <typename Cont>
        auto with_continuation(Cont&& cont) &&
        {
            return node<Cont>(m_delay, voy_fwd(cont));
        }

    private:
        std::chrono::milliseconds m_delay;
    };

} // namespace detail

template<typename T>
decltype(auto) debounce(std::chrono::milliseconds delay)
{
    return detail::debounce_impl<T>(delay);
}

} // namespace voy

#endif // include guard

