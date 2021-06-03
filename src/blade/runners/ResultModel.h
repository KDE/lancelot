/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

