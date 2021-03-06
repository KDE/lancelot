/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

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
#include <voy/dsl/multiprocess.h>

#include "../../utils/debug.h"

using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;


#if defined TEST_FRONTEND
voy_declare_bridge_spread(frontend_to_backend_1)
voy_declare_bridge_ignored(backend_1_to_backend_2)
voy_declare_bridge_in(backend_2_to_frontend)

#elif defined TEST_BACKEND_1
voy_declare_bridge_accept(frontend_to_backend_1)
voy_declare_bridge_post(backend_1_to_backend_2)
voy_declare_bridge_ignored(backend_2_to_frontend)

#else // TEST_BACKEND_2
voy_declare_bridge_ignored(frontend_to_backend_1)
voy_declare_bridge_collect(backend_1_to_backend_2)
voy_declare_bridge_out(backend_2_to_frontend)

#endif

inline std::string pid_s()
{
    return " // pid:" + debug::colorize(getpid());
}

int main(int argc, char *argv[])
{
    using voy::dsl::operator|;
    using voy::dsl::operator||;
    using debug::color;

    auto cout = [] (auto&& value) {
        std::cout << "Out: " << voy_fwd(value) << std::endl;
    };

    auto append_pid = voy::transform([] (std::string&& value) {
                debug::out(color::gray) << "sending " << value << " from " << pid_s();
                return std::move(value) + pid_s();
            });

    debug::out(color::red) << pid_s();

    auto pipeline =
        voy::system_cmd("ping"s, "localhost"s)
        | voy::transform([] (std::string&& value) {
              std::transform(value.begin(), value.end(), value.begin(), toupper);
              debug::out(color::gray) << value << pid_s();
              return value;
          })
        | append_pid
        | voy_bridge(frontend_to_backend_1)

        | voy::transform([] (std::string&& value) {
              const auto pos = value.find_last_of('=');
              debug::out(color::gray) << value << " found = at " << pos << pid_s();
              return std::make_pair(std::move(value), pos);
          })
        | voy::transform([] (std::pair<std::string, size_t>&& pair) {
              const auto& [ value, pos ] = pair;
              debug::out(color::gray) << value << " found = at " << pos << " - extracting ms" << pid_s();
              return pos == std::string::npos
                          ? value
                          : std::string(value.cbegin() + pos + 1, value.cend());
          })
        | append_pid
        | voy_bridge(backend_1_to_backend_2)

        | voy::filter([] (const std::string& value) {
              debug::out(color::gray) << value << " - filtering" << pid_s();
              return value < "0.145"s;
          })
        | append_pid
        | voy_bridge(backend_2_to_frontend)

        | voy::sink{cout};

    voy::event_loop::run();

    return 0;
}

