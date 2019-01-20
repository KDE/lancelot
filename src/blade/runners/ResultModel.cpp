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

#include "ResultModel.h"

ResultModel::ResultModel()
{
    QObject::connect(&m_collector, &Agents::Collector::newResultsAppeared,
                     this  ,       &ResultModel::addResults);
}

int ResultModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return m_currentResults.size();
}

QVariant ResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};

    const auto row = index.row();

    if (row < 0 || row >= m_currentResults.size()) return {};

    const auto &result = m_currentResults[row];

    switch (role) {
    case ResultId:
        return result.url;

    case ResultTitle:
        return result.title;

    case ResultDescription:
        return result.description;

    case ResultIcon:
        return result.icon;

    case ResultRelevance:
        return (qreal)result.relevance;

    case ResultMatchedText:
        return result.matchedText;

    case ResultMimeType:
        return result.mimeType;

    default:
        return {};

    }
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation,
                    int role) const
{
    return "";
}

QHash<int, QByteArray> ResultModel::roleNames() const
{
    return {
        { ResultId          , "id" },

        { ResultTitle       , "title" },
        { ResultDescription , "description" },
        { ResultIcon        , "icon" },

        { ResultUrl         , "url" },
        { ResultRelevance   , "relevance" },
        { ResultMatchedText , "matchedText" },
        { ResultMimeType    , "mimeType" }
    };
}

QString ResultModel::queryString() const
{
    return m_collector.queryString();
}

void ResultModel::setQueryString(const QString &queryString)
{
    if (queryString == m_collector.queryString()) {
        return;
    }

    if (queryString.isEmpty()) {
        if (m_currentResults.size() > 0) {
            beginResetModel();
            m_currentResults.clear();
            endResetModel();
        }

    } else {
        // When the query string changes, we need to go through
        // our list of items and remove those that do not contain
        // this string.
        // The alternative is to remove all the items, but this
        // will have nicer behaviour for the user - if we found
        // Konsole when the user typed 'kon' it will not be removed
        // if the user adds 's'.
        for (int row = m_currentResults.size() -1 ; row >= 0; --row) {
            // TODO: Some runners do matching by words, some by phrases
            //       how it should be handled here?
            if (!m_currentResults[row].matchedText.contains(queryString, Qt::CaseInsensitive)) {
                removeRow(row);
            }
        }

        // The query has changed. If we are keeping any items,
        // we are settinh their relevance to zero. This way,
        // if the runners do not return them again, they will
        // end up at the end of the list
        for (auto& currentResult: m_currentResults) {
            currentResult.relevance = 0;
        }

        qDebug() << "ResultModel - query string changed to" << queryString
                 << ", kept" << m_currentResults;
    }

    m_collector.setQueryString(queryString);
    emit queryStringChanged(queryString);
}

void ResultModel::addResults(const ResultList &newResults)
{
    qDebug() << "ResultModel - adding results" << newResults <<
                "to the" << m_currentResults;

    for (auto newResult: newResults) {
        // Check whether this one is already in the list.
        // If it is, move it to its rightful place based
        // on the relevance.
        const auto oldPositionIter
            = Result::findResult(m_currentResults, newResult.url);

        if (oldPositionIter != m_currentResults.cend()) {
            const auto oldPosition = rowFor(oldPositionIter);

            // If more than one runner returned this result, lets
            // combine the relevance
            qDebug() << "Old relevance: " << newResult.relevance
                     << "Previous relevance: " << oldPositionIter->relevance
                     << "New relevance: " << (newResult.relevance || oldPositionIter->relevance);
                     ;
            newResult.relevance
                = newResult.relevance || oldPositionIter->relevance;

            if (newResult.relevance == oldPositionIter->relevance) {
                continue;
            }

            // We have found the item. If the new score is greater,
            // it should be moved up, otherwise down.
            if (Result::betterThan(newResult, *oldPositionIter)) {
                auto newPositionIter = destinationFor(newResult);

                oldPositionIter->relevance = newResult.relevance;

                // We are at a point where the item pointed at by
                // newPositionIter is not better than our current item
                moveRow(oldPosition,
                        rowFor(newPositionIter));
            } else {
                qFatal("This should not happen - we should always move things "
                       "upwards");
            }

        } else {
            auto newPositionIter = destinationFor(newResult);
            insertRow(rowFor(newPositionIter), newResult);

        }

    }
}

ResultList::iterator ResultModel::destinationFor(const Result &result)
{
    auto destination = m_currentResults.begin();

    // This is a small collection, linear search will
    // probably be faster than binary
    const auto end = m_currentResults.end();
    while (destination != end && Result::betterThan(*destination, result)) {
        ++destination;
    }

    return destination;
}

void ResultModel::removeRow(int position)
{
    beginRemoveRows(QModelIndex(), position, position);
    m_currentResults.removeAt(position);
    endRemoveRows();
}

void ResultModel::insertRow(int position, const Result &result)
{
    beginInsertRows(QModelIndex(), position, position);
    m_currentResults.insert(position, result);
    endInsertRows();
}

void ResultModel::moveRow(int from, int to)
{
    if (from == to) return;

    beginMoveRows(QModelIndex(), from, from,
                  QModelIndex(), to);

    if (from > to) {
        //  _____.......#___
        //       `to    `from
        //               `from + 1
        std::rotate(iterFor(to),
                    iterFor(from),
                    iterFor(from + 1));
    } else if (from < to) {
        //  _____#......____
        //       `from  `to
        //        `from + 1
        std::rotate(iterFor(from),
                    iterFor(from + 1),
                    iterFor(to));
    }

    endMoveRows();
}

int ResultModel::rowFor(ResultList::const_iterator iter) const
{
    return std::distance(m_currentResults.cbegin(), iter);
}

ResultList::iterator ResultModel::iterFor(int row)
{
    return m_currentResults.begin() + row;
}
