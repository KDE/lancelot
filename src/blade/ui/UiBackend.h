/*
    SPDX-FileCopyrightText: 2018 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef BLADE_UIBACKEND_H
#define BLADE_UIBACKEND_H

#include <QObject>

class UiBackend: public QObject {
    Q_OBJECT

public Q_SLOTS:
    void search(const QString &query);

Q_SIGNALS:
    void searchRequested(const QString &query);

    void searchFinished(const QString &query);

private:

};

#endif // include guard

