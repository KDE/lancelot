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

#ifndef VOY_TCP_SERVICE_H
#define VOY_TCP_SERVICE_H

// STL
#include <memory>
#include <string>

// Boost
#include <boost/asio.hpp>

// Self
#include "../utils.h"
#include "../traits.h"
#include "../dsl/node_tags.h"
#include "../dsl/node_traits.h"
#include "../engine/asio/service.h"
#include "../engine/asio/tcp/service.h"

namespace voy::tcp {

using voy::utils::non_copyable;

using voy::dsl::continuator_base,
      voy::dsl::source_node_tag;

template <typename T = std::string>
class service: non_copyable {
    voy_assert_value_type(T);

public:
    using node_category = source_node_tag;

    explicit service(unsigned short port)
        : m_port{port}
    {
    }

    template <typename Cont>
    class node: continuator_base<Cont>, non_copyable {
        using base = continuator_base<Cont>;

    private:
        unsigned short m_port;
        std::unique_ptr<
            engine::asio::tcp::service<node>> m_service;

    public:
        node(unsigned short port, Cont&& cont)
            : base{std::move(cont)}
            , m_port(port)
        {
        }

        void operator() (const voy::engine::asio::tcp::with_client<std::string>& line) const
        {
            if constexpr (std::is_same_v<T, std::string>) {
                base::emit(line);
            } else {
                // TODO: Convert string to T
            }
        }

        void init()
        {
            m_service = std::make_unique<engine::asio::tcp::service<node>>(
                    engine::asio::service::instance(), m_port, *this);
        }
    };

    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node<Cont>(m_port, voy_fwd(cont));
    }

private:
    unsigned short m_port;

};

} // namespace voy::tcp

#endif // include guard

