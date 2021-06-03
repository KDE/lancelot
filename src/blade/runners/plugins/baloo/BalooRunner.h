/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef BALOO_RUNNER_H
#define BALOO_RUNNER_H

#include <AbstractRunner.h>

class BalooRunner: public AbstractRunner {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.blade.Baloo")

public:
    BalooRunner(QObject *parent, const QVariantList &args);
    ~BalooRunner();

    void query() override;

    void queryType(const QString &type, const QString &localizedType);

private:
};


#endif // include guard end

