/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_TRASNPORT_ASIO_SERVICE_H
#define VOY_TRASNPORT_ASIO_SERVICE_H

// Boost
#include <boost/asio/io_service.hpp>

namespace voy::engine::asio {

class service {
public:
    static service& instance();

    void run();

    template <typename F>
    inline void invoke_later(F&& f)
    {
        m_asio_service.post(std::forward<F>(f));
    }

    inline operator boost::asio::io_service& ()
    {
        return m_asio_service;
    }

private:
    boost::asio::io_service m_asio_service;
};

} // namespace voy::engine::asio

#endif // include guard

