/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef RECOLL_RUNNER_H
#define RECOLL_RUNNER_H

#include <AbstractRunner.h>

class RecollRunner: public AbstractRunner {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.blade.Recoll")

public:
    RecollRunner(QObject *parent, const QVariantList &args);
    ~RecollRunner();

    void query() override;

private:
    class Private;
    Private * const d;
};


#endif // include guard end

