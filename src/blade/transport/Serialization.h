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

#ifndef BLADE_TRANSPORT_H
#define BLADE_TRANSPORT_H

#include "ControllerMessage.h"

#include <sstream>
#include <iostream>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include <kj/std/iostream.h>

#include <utils/overloaded.h>
using std_ex::overloaded;

#include "transport/protocol/controller_message.capnp.h"

namespace blade::serialization {

inline
void writeToStdStream(const ControllerMessage &msg, std::ostream& out_)
{
    capnp::MallocMessageBuilder builder;

    auto cm = builder.initRoot<protocol::ControllerMessage>();

    cm.setHost(msg.host.data());
    cm.setController(msg.controller.data());

    auto message = cm.initMessage();

    std::visit(
        overloaded {
            [&](const PingMessage &) { message.initPing(); },
            [&](const QueryMessage &msg) {
                auto query = message.initQuery();
                query.setId(msg.id);
                query.setText(msg.text.toUtf8().data());
            },
            [&](const ErrorMessage &msg) {
                auto error = message.initError();
                error.setCode(msg.code);
                error.setMessage(msg.message.data());
            },
        },
        msg.message);

    kj::std::StdOutputStream out(out_);
    capnp::writeMessage(out, builder);
}

void writeToStdStream(const ControllerMessage &msg, std::ostream& out);

inline
std::string asStdString(const ControllerMessage& msg)
{
    std::ostringstream result;
    writeToStdStream(msg, result);
    return result.str();
}

namespace detail {
    auto asByteArray(const capnp::Text::Reader& reader)
    {
        return QByteArray(reader.cStr(), reader.size());
    }
} // namespace detail

inline
blade::ControllerMessage readControllerMessage(const std::string &data)
{
    std::istringstream ss(data);
    kj::std::StdInputStream is(ss);
    capnp::InputStreamMessageReader reader(is);

    auto cm = reader.getRoot<protocol::ControllerMessage>();

    blade::ControllerMessage result;

    result.host = detail::asByteArray(cm.getHost());
    result.controller = detail::asByteArray(cm.getController());

    auto message = cm.getMessage();

    if (message.hasQuery()) {
        auto query = message.getQuery();
        result.message = blade::QueryMessage{
            query.getId(),
            QString::fromUtf8(detail::asByteArray(query.getText()))
        };

    } else if (message.hasPing()) {
        result.message = blade::PingMessage{};

    } else if (message.hasError()) {
        auto error = message.getError();
        result.message = blade::ErrorMessage{
            error.getCode(),
            detail::asByteArray(error.getMessage())
        };

    } else {
        result.message = blade::ErrorMessage{
            static_cast<std::uint16_t>(-1),
            "unknown error"
        };
    }

    return result;
}


} // namespace blade::serialization

#endif // include guard

