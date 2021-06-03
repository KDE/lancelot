/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "UiBackend.h"

#include <QDebug>

void UiBackend::search(const QString &query)
{
    Q_EMIT searchRequested(query);
}

