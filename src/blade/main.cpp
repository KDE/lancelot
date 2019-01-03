/*
 *   Copyright (C) 2018, 2019 Ivan Čukić <ivan.cukic(at)kde.org>
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

#include <iostream>

#include "Protocol.h"
#include "ControllerMessage.h"
#include "ui/UiBackend.h"

#include <utils/qstringliterals.h>

#include <voy/basic/sink.h>
#include <voy/basic/values.h>
#include <voy/basic/delayed.h>

#include <voy/operations/transform.h>

#include <voy/dsl.h>

#include <voy/wrappers/qt/connect.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QThread>

#include <future>

#include <boost/thread/thread.hpp>

using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;

class QueryGenerator {
public:
    template <typename T>
    blade::QueryMessage operator() (T&& value) const
    {
        return { m_id, voy_fwd(value) };
    }

private:
    mutable std::uint64_t m_id = 0;
};

int main(int argc, char *argv[])
{
    using namespace voy;

    // using blade::ControllerMessage;
    // using blade::PingMessage;
    // using blade::QueryMessage;
    //
    // ControllerMessage cm;
    // cm.host = "Host";
    // cm.controller = "Cler";
    // cm.message = QueryMessage{ 1 , "GGG"_qs };
    //
    // blade::serialize(cm);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    UiBackend backend;

    auto context = engine.rootContext();
    context->setContextProperty("BladeUiBackend", &backend);

    engine.load(app.arguments()[1]);
    if (engine.rootObjects().isEmpty()) throw "QML not loaded properly";

    boost::thread voy_thread([&backend] {
        auto cout = [] (auto&& value) {
            qDebug() << "SINK: " << value;
        };

        using voy::dsl::operator|;

        auto pipeline =
            qt::signal(&backend, &UiBackend::searchRequested)
                | transform(QueryGenerator{})
                | transform([] (auto&& query) {
                      return blade::ControllerMessage { "42", "0", voy_fwd(query) };
                  })
                | transform(blade::serialization::asStdString)
                | transform(blade::serialization::readControllerMessage)
                | transform([] (auto&& cm) {
                      qDebug() << cm;
                      return QString::fromLatin1(cm.host);
                  })
                | qt::slot(&backend, &UiBackend::searchFinished);

        voy::event_loop::run();
    });

    return app.exec();
}

