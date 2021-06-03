/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef BLADE_ABSTRACT_RUNNER_H
#define BLADE_ABSTRACT_RUNNER_H

#include <runners/Result.h>
#include <QCoreApplication>

// #include "blade_plugin_export.h"

// #define BLADE_EXPORT_PLUGIN(libname, classname, jsonFile) \
//         K_PLUGIN_FACTORY_WITH_JSON(factory, jsonFile, registerPlugin<classname>();)

#define BLADE_PLUGIN_EXPORT

class BLADE_PLUGIN_EXPORT AbstractRunner: public QObject {
    Q_OBJECT

public:
    AbstractRunner(QObject *parent);
    virtual ~AbstractRunner();

    void runQuery(const QString &queryString);
    virtual void cancelQuery();

protected:
    virtual void query() = 0;

    inline void yield() const {
        QCoreApplication::processEvents();
    }

    void reportNewResults(const ResultList &results);

    QString queryString() const;

Q_SIGNALS:
    void startedProcessingQuery();
    void newResultsAppeared(const ResultList &results);
    void finishedProcessingQuery();

private:
    QString m_queryString;

};

#endif // include guard end

