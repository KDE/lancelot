/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef UTILS_OVERLOADED_H
#define UTILS_OVERLOADED_H

namespace std_ex {

template <typename... Ops>
struct overloaded: Ops... {
    using Ops::operator()...;
};

template <typename... Ops>
overloaded(Ops...) -> overloaded<Ops...>;

} // namespace std_ex

#endif // include guard

