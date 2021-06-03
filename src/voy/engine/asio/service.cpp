/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "service.h"

#include <iostream>

namespace voy::engine::asio {

service& service::instance()
{
    static service s_instance;
    return s_instance;
}

void service::run()
{
    boost::asio::io_service::work keep_alive(m_asio_service);
    m_asio_service.run();
}

} // namespace voy::transport::asio

