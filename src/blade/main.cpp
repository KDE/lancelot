/*
    SPDX-FileCopyrightText: 2018, 2019 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
#include "transport/ControllerMessage.h"
#include "ui/UiBackend.h"
namespace s11n = blade::serialization;

// Utils
#include <utils/qstringliterals.h>
#include <utils/overloaded.h>
#include <utils/constructor_fn.h>
#include <utils/bind_front.h>
using std_ex::overloaded;
using std_ex::bind_front;

// Runners
#include "runners/AbstractRunner.h"
#include "runners/plugins/services/ServicesRunner.h"

using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;

using namespace voy;


class QueryGenerator {
public:
    template <typename T>
    blade::QueryMessage operator() (T&& value) const
    {
        ++m_id;
        return { m_id, voy_fwd(value) };
    }

private:
    mutable std::uint64_t m_id = 0;
};


class RunnerManager {
    using runners_t = std::vector<std::unique_ptr<AbstractRunner>>;

public:
    RunnerManager()
    {
        m_runners.emplace_back(std::make_unique<ServicesRunner>());
    }

    void runQuery(const QString &queryString)
    {
        for (const auto& runner: m_runners) {
            runner->runQuery(queryString);
        }
    }

private:
    runners_t m_runners;
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

        RunnerManager rm;

        using voy::dsl::operator|;

        auto pipeline =
            qt::signal(&backend, &UiBackend::searchRequested)
                | remove_if(&QString::isEmpty)
                | debounce<QString>(200ms)
                | transform(QueryGenerator{})
                | transform(bind_front(initialize<blade::ControllerMessage>{}, "42", "0"))
                // | transform(s11n::asStdString)
                // | transform(s11n::readControllerMessage)
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
                | sink([] (auto&& value) { qDebug() << "Generated:" << value; })
                ;
                // | qt::slot(&backend, &UiBackend::searchFinished);

        voy::event_loop::run();
    });

    return app.exec();
}

