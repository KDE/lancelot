/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "AbstractRunner.h"

#include <QDebug>
#include <QCoreApplication>

AbstractRunner::AbstractRunner(QObject *parent)
    : QObject(parent)
{
}

AbstractRunner::~AbstractRunner()
{
}

void AbstractRunner::reportNewResults(const ResultList &results)
{
    Q_EMIT newResultsAppeared(results);

    yield();
}

void AbstractRunner::cancelQuery()
{
    m_queryString.clear();
}

void AbstractRunner::runQuery(const QString &queryString)
{
    if (m_queryString == queryString) return;

    if (!m_queryString.isEmpty()) {
        cancelQuery();
    }

    m_queryString = queryString;

    query();
}

QString AbstractRunner::queryString() const
{
    return m_queryString;
}

