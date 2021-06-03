/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
