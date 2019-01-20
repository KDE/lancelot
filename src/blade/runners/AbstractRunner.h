/*
 *   Copyright (C) 2016 Ivan Čukić <ivan.cukic(at)kde.org>
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

