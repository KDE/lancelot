/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SERVICES_RUNNER_H
#define SERVICES_RUNNER_H

#include <runners/AbstractRunner.h>

#include <KService>

class ServicesRunner: public AbstractRunner {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.blade.Services")

public:
    ServicesRunner(QObject *parent = nullptr, const QVariantList &args = QVariantList{});
    ~ServicesRunner();

    void query() override;
    void cancelQuery() override;

    void setupResult(const KService::Ptr &service, Result &result);

private:
    QSet<QString> m_seen;
};


#endif // include guard end

