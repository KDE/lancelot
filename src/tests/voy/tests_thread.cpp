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

