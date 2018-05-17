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

#ifndef VOY_ZMQ_SERVICE_H
#define VOY_ZMQ_SERVICE_H

// STL
#include <memory>
#include <string>
#include <iostream>

// Boost and ZMQ
#include <boost/asio.hpp>
#include <azmq/socket.hpp>

// Self
#include "../utils.h"
#include "../traits.h"
#include "../dsl/node_tags.h"
#include "../dsl/node_traits.h"
#include "../engine/asio/service.h"

#include "../../utils/debug.h"

namespace voy::zmq {

using voy::utils::non_copyable;

using voy::dsl::continuator_base,
      voy::dsl::source_node_tag;

namespace policy {

    struct spread {
        template <typename Socket>
        static void publisher_init(Socket& socket, const std::string& path)
        {
            debug::out() << "spread/publisher binding to " << path;
            socket.bind(path);
        }

        template <typename Socket>
        static void subscriber_init(Socket& socket, const std::string& path)
        {
            debug::out() << "spread/subscriber connecting to " << path;
            socket.connect(path);
            socket.set_option(azmq::socket::subscribe(""));
        }
    };

    struct collect {
        template <typename Socket>
        static void publisher_init(Socket& socket, const std::string& path)
        {
            debug::out() << "collect/publisher connecting to " << path;
            socket.connect(path);
        }

        template <typename Socket>
        static void subscriber_init(Socket& socket, const std::string& path)
        {
            debug::out() << "collect/publisher binding to " << path;
            socket.bind(path);
            socket.set_option(azmq::socket::subscribe(""));
        }
    };
}


std::string ipc(const std::string& id)
{
    static const char *userenv = getenv("USER");
    std::string user = userenv ? userenv : "unknown-user";
    return "ipc:///tmp/voy-zmq-" + std::string(user) + "-" + id;
}


template <typename Policy = policy::spread>
class publisher: non_copyable {
public:
    using node_category = sink_node_tag;

    explicit publisher(std::string path)
        : m_path{std::move(path)}
    {
    }

    template <typename Msg>
    void operator()(Msg&& value) const
    {
        // std::cerr << "Sending... " << value << std::endl;
        if (m_socket) {
            m_socket->async_send(voy_fwd(azmq::message(value)),
                    [] (auto, auto) {});
        }
    }

    void init()
    {
        m_socket = std::make_unique<azmq::pub_socket>(engine::asio::service::instance());

        Policy::publisher_init(*m_socket, m_path);
    }

    void notify_ended() const
    {
        m_socket.reset();
    }

private:
    std::string m_path;
    mutable std::unique_ptr<azmq::pub_socket> m_socket;

};


template <typename Policy = policy::spread>
class subscriber: non_copyable {
public:
    using node_category = source_node_tag;

    explicit subscriber(std::string path)
        : m_path{std::move(path)}
    {
    }

    template <typename Cont>
    class node: continuator_base<Cont>, non_copyable {
        using base = continuator_base<Cont>;

    private:
        struct impl {
            impl(node* parent, const std::string& path)
                : q{parent}
                , socket(engine::asio::service::instance())
            {
                std::cerr << "AZMQ: Connecting to " << path << " ...\n";
                Policy::subscriber_init(socket, path);

                read_next();
            }

            void read_next()
            {
                socket.async_receive([this] (const boost::system::error_code& error,
                                             azmq::message& msg,
                                             size_t bytes_transferred) {
                        if (!error) {
                            q->base::emit(msg.string());
                            // q->base::emit(msg);
                            read_next();
                        }
                    });
            }

            node* const q;
            azmq::sub_socket socket;
            boost::asio::streambuf data;
        };

        std::unique_ptr<impl> d;
        std::string m_path;

    public:
        node(std::string path, Cont&& cont)
            : base{std::move(cont)}
            , m_path{std::move(path)}
        {
        }

        void init()
        {
            d = std::make_unique<impl>(this, m_path);
            base::init();
        }
    };

    template <typename Cont>
    auto with_continuation(Cont&& cont) &&
    {
        return node<Cont>(std::move(m_path), voy_fwd(cont));
    }

private:
    std::string m_path;

};

} // namespace voy::zmq

#endif // include guard

