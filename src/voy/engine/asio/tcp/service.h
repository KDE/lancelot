/*
    SPDX-FileCopyrightText: 2017, 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_ENGINE_ASIO_TCP_SERVICE_H
#define VOY_ENGINE_ASIO_TCP_SERVICE_H

// Standard library
#include <iostream>
#include <functional>

// Boost
#include <boost/asio.hpp>

// Solf
#include "../../../utils.h"

namespace voy::engine::asio::tcp {


// Not a good idea to add a using declaration in a header,
// but we can live with it for this small example
using boost::asio::ip::tcp;

template <typename MessageType>
class with_client {
private:
    MessageType m_value;
    boost::asio::ip::tcp::socket* m_socket;

public:
    with_client(MessageType value, tcp::socket* socket)
        : m_value{std::move(value)}
        , m_socket{socket}
    {
    }

    void reply(const std::string& message) const
    {
        auto buf = std::make_shared<std::string>(message);
        boost::asio::async_write(
            *m_socket,
            boost::asio::buffer(*buf, buf->length()),
            [buf](auto, auto) {});
    }

    const MessageType& operator*() const
    {
        return m_value;
    }

    const MessageType* operator->() const
    {
        return &m_value;
    }
};

namespace detail {


    template <typename MessageType>
    auto make_with_client(MessageType&& value, tcp::socket* socket)
    {
        return voy::engine::asio::tcp::with_client<MessageType>{
            std::forward<MessageType>(value), socket};
    }


    template <typename EmitFunction>
    class session: public std::enable_shared_from_this<session<EmitFunction>> {
    public:
        session(tcp::socket&& socket, EmitFunction emit)
            : m_socket(std::move(socket))
            , m_emit(emit)
        {
        }

        void start()
        {
            do_read();
        }

    private:
        using shared_session = std::enable_shared_from_this<session<EmitFunction>>;

        void do_read()
        {
            // Getting a shared pointer to this instance
            // to capture it in the lambda
            auto self = shared_session::shared_from_this();
            boost::asio::async_read_until(
                m_socket, m_data, '\n',
                [this, self](const boost::system::error_code& error,
                             std::size_t size) {
                    if (!error) {
                        // Reading a line from the client and
                        // passing it to whoever listens to us
                        std::istream is(&m_data);
                        std::string line;
                        std::getline(is, line);
                        m_emit(make_with_client(std::move(line), &m_socket));

                        // Scheduling the next line to be read
                        do_read();
                    }
                });
        }

        tcp::socket m_socket;
        boost::asio::streambuf m_data;
        EmitFunction m_emit;
    };


    template <typename Socket, typename EmitFunction>
    auto make_shared_session(Socket&& socket, EmitFunction&& emit)
    {
        return std::make_shared<session<EmitFunction>>(
                std::forward<Socket>(socket),
                std::forward<EmitFunction>(emit));
    }

} // namespace detail

/**
 * The service class handles client connections
 * and emits the messages sent by the clients
 */
template <typename Cont>
class service: utils::non_copyable {
public:
    explicit service(boost::asio::io_service& service,
                     unsigned short port,
                     Cont& cont)
        : m_acceptor(service, tcp::endpoint(tcp::v4(), port))
        , m_socket(service)
        , m_emit{cont}
    {
        do_accept();
    }

private:
    void do_accept()
    {
        m_acceptor.async_accept(
            m_socket, [this] (const boost::system::error_code& error) {
                if (!error) {
                    // Creating a new session and start listing for
                    // client messages
                    detail::make_shared_session(
                            std::move(m_socket),
                            m_emit
                        )->start();

                } else {
                    // If there was a connection error,
                    // just write it out
                    std::cerr << error.message() << std::endl;

                }

                // Listening to another client
                do_accept();
            });
    }

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
    Cont& m_emit;
};

} // namespace voy::engine::asio::tcp

#endif // include guard
