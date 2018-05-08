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

#ifndef VOY_TRASNPORT_ASIO_PROCESS_H
#define VOY_TRASNPORT_ASIO_PROCESS_H

// STL
#include <string>
#include <iostream>

// Boost
#include <boost/process.hpp>
#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/asio/read_until.hpp>

// Self
#include "utils.h"
#include "service.h"

namespace voy::engine::asio {

using utils::non_copyable;

template <typename Handler>
class process: non_copyable {
public:
    template <typename Tuple, size_t ...Idx>
    auto make_process_impl(Tuple&& args, std::index_sequence<Idx...>)
    {
        // utils::print_types<Tuple>();
        return boost::process::child(std::get<Idx>(std::move(args))..., boost::process::std_out > m_pipe);
    }

    template <typename ...Args>
    decltype(auto) make_process(std::tuple<Args...> args)
    {
        return make_process_impl(std::move(args),
                                 std::index_sequence_for<Args...>());
    }



    template <typename ...Args>
    process(Handler handler, std::tuple<Args...> args)
        : m_handler{std::move(handler)}
        , m_pipe(service::instance())
        , m_process{make_process(args)}
        // , m_process(args, boost::process::std_out > m_pipe)
    {
        read_next();
    }

    ~process()
    {
    }

    process(process&& other) = delete;
    process& operator=(process&& other) = delete;

    template <typename T>
    void notify(T&& value)
    {
        voy_fwd_invoke(m_handler, value);
    }

    void init_handler()
    {
        m_handler.init();
    }

    void read_next()
    {
        boost::asio::async_read_until(
                m_pipe, m_buffer, "\n",
                [&] (boost::system::error_code ec, size_t count) {
                    // std::cerr << "One read finished: " << ec.message() << " count:" << count << std::endl;

                    if (ec && !(ec.value() == boost::asio::error::eof)) {
                        std::cerr << "Unknown error: " << ec.message() << std::endl;
                        m_handler.notify_ended();
                        return;
                    }

                    // std::string line;
                    std::istream stream(&m_buffer);
                    auto line_begin = std::istreambuf_iterator<char>(stream);
                    const auto buffer_end = std::istreambuf_iterator<char>();

                    // std::string s(line_begin, buffer_end);
                    // std::cerr << "READ [" << s << "]\n";

                    for (;;) {
                        auto is_newline = [] (char c) { return c == '\n'; };

                        std::string line;
                        line.reserve(256);

                        auto [ next, output ] = boost::algorithm::copy_until(
                                line_begin, buffer_end,
                                std::back_inserter(line),
                                is_newline);

                        line_begin = next;

                        if (buffer_end == line_begin) {
                            m_previous_line = line;
                            break;
                        }

                        auto to_send = m_previous_line + line;
                        std::invoke(m_handler, to_send);
                        m_previous_line.clear();

                        ++line_begin;

                        if (buffer_end == line_begin) {
                            break;
                        }
                    }

                    if (!ec) {
                        read_next();

                    } else {
                        std::invoke(m_handler, std::move(m_previous_line));
                        m_handler.notify_ended();
                    }
                });
    }

private:
    Handler m_handler;
    boost::process::async_pipe m_pipe;
    boost::process::child m_process;
    boost::asio::streambuf m_buffer;
    std::string m_previous_line;
};

} // namespace voy::engine::asio

#endif // include guard

