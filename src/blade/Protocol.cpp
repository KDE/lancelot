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

#include "Protocol.h"

#include <iostream>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <kj/std/iostream.h>

#include <utils/overloaded.h>

#include "protocol/controller_message.capnp.h"

namespace blade {

void serialize(const ControllerMessage &msg)
{
    capnp::MallocMessageBuilder builder;

    auto cm = builder.initRoot<protocol::ControllerMessage>();

    cm.setHost(msg.host.data());
    cm.setController(msg.controller.data());

    auto message = cm.initMessage();

    std::visit(
        overloaded{
            [&](const PingMessage &) { message.initPing(); },
            [&](const QueryMessage &msg) {
                auto query = message.initQuery();
                query.setId(msg.id);
                query.setText(msg.text.toUtf8().data());
            },
            [&](const ErrorMessage &msg) {
                auto error = message.initError();
                error.setCode(msg.code);
                error.setMessage(msg.message.toUtf8().data());
            },
        },
        msg.message);

    std::cout << "----[";
    kj::std::StdOutputStream out(std::cout);
    capnp::writeMessage(out, builder);
    std::cout << "]----\n";

}

} // namespace blade

