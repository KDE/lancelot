/*
    SPDX-FileCopyrightText: 2019 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

