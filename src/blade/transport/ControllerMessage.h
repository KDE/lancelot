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

