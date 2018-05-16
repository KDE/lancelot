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

#include <iostream>

#include "operations/merge.h"
#include "operations/slice.h"
#include "operations/transform.h"
#include "operations/filter.h"
#include "operations/identity.h"

#include "basic/delayed.h"
#include "basic/values.h"
#include "basic/sink.h"

#include "wrappers/process.h"
#include "wrappers/tcp_service.h"
#include "wrappers/zmq_service.h"

#include "engine/event_loop.h"

#include "dsl.h"
#include "dsl/multiprocess.h"

#include "../utils/debug.h"

using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;


#if defined TEST_FRONTEND
voy_declare_bridge_out(frontend_to_backend_1)
voy_declare_bridge_ignored(backend_1_to_backend_2)
voy_declare_bridge_in(backend_2_to_frontend)

#elif defined TEST_BACKEND_1
voy_declare_bridge_in(frontend_to_backend_1)
voy_declare_bridge_out(backend_1_to_backend_2)
voy_declare_bridge_ignored(backend_2_to_frontend)

#else // TEST_BACKEND_2
voy_declare_bridge_ignored(frontend_to_backend_1)
voy_declare_bridge_in(backend_1_to_backend_2)
voy_declare_bridge_out(backend_2_to_frontend)

#endif


int main(int argc, char *argv[])
{
    auto cout = [] (auto&& value) {
        std::cout << "Out: " << voy_fwd(value) << std::endl;
    };

    using voy::dsl::operator|;
    using voy::dsl::operator||;

    auto pipeline =
        voy::system_cmd("ping"s, "localhost"s)
        | voy::transform([] (std::string&& value) {
              std::transform(value.begin(), value.end(), value.begin(), toupper);
              return value;
          })
        | voy_bridge(frontend_to_backend_1)
        | voy::transform([] (std::string&& value) {
              const auto pos = value.find_last_of('=');
              return std::make_pair(std::move(value), pos);
          })
        | voy::transform([] (std::pair<std::string, size_t>&& pair) {
              auto [ value, pos ] = pair;
              return pos == std::string::npos
                          ? std::move(value)
                          : std::string(value.cbegin() + pos + 1, value.cend());
          })
        | voy_bridge(backend_1_to_backend_2)
        | voy::filter([] (const std::string& value) {
              return value < "0.145"s;
          })
        | voy_bridge(backend_2_to_frontend)
        | voy::sink{cout};

    voy::event_loop::run();

    return 0;
}

