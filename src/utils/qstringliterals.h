/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef UTILS_QSTRINGLITERALS_H
#define UTILS_QSTRINGLITERALS_H

QString operator"" _qs(const char* data, size_t size) noexcept
{
    return QString::fromUtf8(data, size);
}

#endif // include guard

