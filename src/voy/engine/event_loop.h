/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef VOY_EVENT_LOOP_H
#define VOY_EVENT_LOOP_H

#include "asio/service.h"

namespace voy::event_loop {

using voy::engine::asio::service;

inline void run()
{
    service::instance().run();
}

template <typename F>
inline void invoke_later(F&& f)
{
    service::instance().invoke_later(std::forward<F>(f));
}

} // namespace voy::event_loop

#endif // include guard

