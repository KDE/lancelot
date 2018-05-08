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

#include "basic/delayed.h"
#include "wrappers/process.h"

#include "engine/event_loop.h"

#include "basic/values.h"
#include "basic/sink.h"


#include "dsl.h"

int main(int argc, char *argv[])
{
    auto cout = [] (auto&& value) {
        std::cout << "Out: " << voy_fwd(value) << std::endl;
    };

    using voy::dsl::operator|;
    using namespace std::literals::string_literals;
    using namespace std::literals::chrono_literals;

// #define VALUES_TEST
#ifdef VALUES_TEST
    auto pipeline_values =
        voy::values{42, 6} | voy::sink{cout};
#endif

// #define PROCESS_TEST
#ifdef PROCESS_TEST
    auto pipeline_process =
        voy::system_cmd("task"s) | voy::sink{cout};
#endif

#define DELAYED_TEST
#ifdef DELAYED_TEST
    auto pipeline_delayed =
        voy::delayed(5s, "I'm finally here"s) | voy::sink{cout};
#endif

#define DELAYED_VALS_TEST
#ifdef DELAYED_VALS_TEST
    auto pipeline_delayed_values =
        voy::delayed_values(2s, {"I'm running late"s, "sorry..."s}) | voy::sink{cout};
#endif

    voy::event_loop::run();

    return 0;
}

