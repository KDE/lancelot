/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef ACTIVITIES_RUNNER_H
#define ACTIVITIES_RUNNER_H

#include <AbstractRunner.h>

#include <KActivities/ActivitiesModel>

class ActivitiesRunner: public AbstractRunner {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasma.blade.Activities")

public:
    ActivitiesRunner(QObject *parent, const QVariantList &args);
    ~ActivitiesRunner();

    void query() override;

private:
    KActivities::ActivitiesModel activities;
    inline QString activityData(int row, int role) const;
};


#endif // include guard end

