/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef BLADE_CONTROLLER_MESSAGE_H
#define BLADE_CONTROLLER_MESSAGE_H

#include <variant>

#include <QString>
#include <QByteArray>
#include <QDebug>

namespace blade {

struct PingMessage {};

inline
QDebug operator<< (QDebug out, const PingMessage& msg)
{
    return out.nospace() << "PingMessage{}";
}



struct QueryMessage {
    std::uint64_t id;
    QString text;
};

inline
QDebug operator<< (QDebug out, const QueryMessage& msg)
{
    return out.nospace() << "QueryMessage{" << msg.id << ", " << msg.text << "}";
}



struct ErrorMessage {
    std::uint16_t code;
    QByteArray message;
};

inline
QDebug operator<< (QDebug out, const ErrorMessage& msg)
{
    return out.nospace() << "ErrorMessage{" << msg.code << ", " << msg.message << "}";
}



struct ControllerMessage {
    QByteArray host;
    QByteArray controller;

    std::variant<
        PingMessage,
        QueryMessage,
        ErrorMessage
    > message;
};

inline
QDebug operator<< (QDebug out, const ControllerMessage& cm)
{
    out.nospace() << "ControllerMessage{" << cm.host << ", " << cm.controller << "}::";
    std::visit([&] (const auto &cm) {
            out.nospace() << cm;
        }, cm.message);
    return out;
}



} // namespace blade

#endif // include guard

