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

// std, boost
#include <iostream>
#include <future>
#include <boost/thread/thread.hpp>

// Voy
#include <voy/basic/sink.h>
#include <voy/basic/values.h>
#include <voy/basic/delayed.h>

#include <voy/dsl.h>
#include <voy/operations/transform.h>
#include <voy/operations/filter.h>
#include <voy/operations/debounce.h>
#include <voy/wrappers/qt/connect.h>

// Qt
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QThread>

// Blade
#include "transport/Serialization.h"
namespace s11n = blade::serialization;
#include "transport/ControllerMessage.h"
#include "ui/UiBackend.h"

// Utils
#include <utils/qstringliterals.h>
#include <utils/overloaded.h>
#include <utils/constructor_fn.h>
#include <utils/bind_front.h>
using std_ex::overloaded;
using std_ex::bind_front;

using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;

using namespace voy;


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
                | remove_if(&QString::isEmpty)
                | debounce<QString>(200ms)
                | transform(QueryGenerator{})
                | transform(bind_front(initialize<blade::ControllerMessage>{}, "42", "0"))
                | transform(s11n::asStdString)
                | transform(s11n::readControllerMessage)
                | transform([] (auto&& cm) {
                      qDebug() << cm;
                      return std::visit(
                          overloaded {
                              [] (const blade::PingMessage& msg) {
                                  return "PING"_qs;
                              },
                              [] (const blade::QueryMessage& msg) {
                                  return msg.text;
                              },
                              [] (const blade::ErrorMessage& msg) {
                                  return QString::fromLatin1(msg.message);
                              }
                          }, cm.message);
                  })
                | qt::slot(&backend, &UiBackend::searchFinished);

        voy::event_loop::run();
    });

    return app.exec();
}

