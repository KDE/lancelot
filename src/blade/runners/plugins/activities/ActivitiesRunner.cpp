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

#include "ActivitiesRunner.h"

#include <QDebug>
#include <KPluginFactory>

using KActivities::ActivitiesModel;

#define EXACT_MATCH_SCORE 1
#define NAME_SCORE .8
#define DESCRIPTION_SCORE .5

BLADE_EXPORT_PLUGIN(activitiesrunner, ActivitiesRunner, "blade-plugin-activities.json")

ActivitiesRunner::ActivitiesRunner(QObject *parent, const QVariantList &args)
    : AbstractRunner(parent)
{
    Q_UNUSED(args)
}

ActivitiesRunner::~ActivitiesRunner()
{
}

QString ActivitiesRunner::activityData(int row, int role) const
{
    const auto index = activities.index(row);
    return activities.data(index, role).toString();
}

void ActivitiesRunner::query()
{
    emit startedProcessingQuery();

    const auto queryWords = queryString().split(" ", QString::SkipEmptyParts);

    ResultList results;

    for (int i = 0; i < activities.rowCount(); ++i) {
        qreal score = 0.0;

        const auto activityId
            = activityData(i, ActivitiesModel::ActivityId);
        const auto activityName
            = activityData(i, ActivitiesModel::ActivityName).toLower();
        const auto activityDescription
            = activityData(i, ActivitiesModel::ActivityDescription).toLower();

        if (activityName == queryString() || activityDescription == queryString()) {
            score = EXACT_MATCH_SCORE;

        } else {
            bool allMatched = true;

            for (const auto &_word: queryWords) {
                auto word = _word.toLower();    // ... I want ranges

                score += activityName == word               ? EXACT_MATCH_SCORE
                       : activityName.contains(word)        ? NAME_SCORE
                       : activityDescription.contains(word) ? DESCRIPTION_SCORE
                       : (allMatched = false);  // this is evil
            }

            if (allMatched) {
                score /= queryWords.count();
            } else {
                score = 0;
            }
        }

        if (score > .2) {
            Result result;

            result.title       = activityName;
            result.description = activityDescription;
            result.matchedText = activityName + " " + activityDescription;
            result.url         = "activities://" + activityId;
            result.relevance   = score;

            results << result;
        }
    }

    emit reportNewResults(results);

    emit finishedProcessingQuery();
}

#include "ActivitiesRunner.moc"

