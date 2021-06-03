/*
    SPDX-FileCopyrightText: 2019 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <iostream>

#include <voy/operations/merge.h>
#include <voy/operations/slice.h>
#include <voy/operations/transform.h>
#include <voy/operations/filter.h>
#include <voy/operations/identity.h>

#include <voy/basic/delayed.h>
#include <voy/basic/values.h>
#include <voy/basic/sink.h>

#include <voy/wrappers/process.h>
#include <voy/wrappers/tcp_service.h>
#include <voy/wrappers/zmq_service.h>

#include <voy/engine/event_loop.h>

#include <voy/dsl.h>

#include <boost/thread/thread.hpp>

int main(int argc, char *argv[])
{
    using voy::dsl::operator|;
    using namespace std::literals::string_literals;
    using namespace std::literals::chrono_literals;

    boost::thread thread([] {
        auto cout = [] (auto&& value) {
            std::cout << "Out: " << voy_fwd(value) << std::endl;
        };

        auto pipeline_delayed =
            voy::delayed(5s, "I'm finally here"s) | voy::sink{cout};

        voy::event_loop::run();
    });

    std::cout << "Doing something\n";

    thread.join();

    return 0;
}

