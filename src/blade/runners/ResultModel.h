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

#ifndef BLADE_RESULT_MODEL_H
#define BLADE_RESULT_MODEL_H

#include <QAbstractListModel>
#include "agents/Collector.h"

#include "messages/ResultMessage.h"

class ResultModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(QString queryString
               READ queryString WRITE setQueryString NOTIFY queryStringChanged)

public:
    enum Roles {
        ResultId          = Qt::UserRole,

        ResultTitle       ,
        ResultDescription ,
        ResultIcon        ,

        ResultUrl         ,
        ResultRelevance   ,
        ResultMatchedText ,
        ResultMimeType
    };

    ResultModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    QString queryString() const;

public Q_SLOTS:
    void setQueryString(const QString &queryString);
    void addResults(const ResultList &results);

Q_SIGNALS:
    void queryStringChanged(const QString &queryString);

private:
    void removeRow(int position);
    void insertRow(int position, const Result &result);
    void moveRow(int from, int to);

    ResultList::iterator destinationFor(const Result &result);

    int rowFor(ResultList::const_iterator iter) const;
    ResultList::iterator iterFor(int row);

private:
    ResultList m_currentResults;
    Agents::Collector m_collector;

};

#endif // include guard end

