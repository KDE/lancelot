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

#ifndef BLADE_RESULT_H
#define BLADE_RESULT_H

#include <QString>
#include <QDataStream>
#include <QVariant>
#include <QList>
#include <QUrl>
#include <QDebug>

#include <runners/FuzzyBool.h>

class Result {
public:
    void serialize(QDataStream &out) const;
    void deserialize(QDataStream &in);
    QDebug &debug(QDebug &out) const;

    // Id of the query we are responding to
    quint32 queryId;

    // User-visible parts
    QString title;
    QString description;
    QString icon;

    // Internal parts
    QUrl url;
    FuzzyBool<qreal> relevance;
    QString matchedText;
    QByteArray mimeType;

    // QVariant data;
    // QString resultId;
    //
    // QByteArray category;
    // int type;

    static inline bool betterThan(const Result &left, const Result &right)
    {
        return left.relevance > right.relevance ? true
             : left.relevance < right.relevance ? false
             : left.title.toLower() < right.title.toLower();
    }

    template <typename Collection>
    static inline typename Collection::iterator
    findResult(Collection &collection, const QUrl &url)
    {
        return std::find_if(collection.begin(), collection.end(),
                [&url] (const Result &result) {
                    return result.url == url;
                });
    }

};

inline
QDebug &operator<< (QDebug debug, const Result &result)
{
    return debug << result.title << "/" << result.relevance;
}

typedef QVector<Result> ResultList;

Q_DECLARE_METATYPE(Result)
Q_DECLARE_METATYPE(ResultList)

#endif // include guard end
