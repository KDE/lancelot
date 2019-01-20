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

#include "BalooRunner.h"

#include <QDebug>
#include <QDir>
#include <KPluginFactory>
#include <QMimeDatabase>

#include <KLocalizedString>

#include <Baloo/Query>

BLADE_EXPORT_PLUGIN(baloorunner, BalooRunner, "blade-plugin-baloo.json")

BalooRunner::BalooRunner(QObject *parent, const QVariantList &args)
    : AbstractRunner(parent)
{
    Q_UNUSED(args)
}

BalooRunner::~BalooRunner()
{
}

void BalooRunner::query()
{
    qDebug() << queryString();
    emit startedProcessingQuery();

    queryType("Document", i18n("Document"));
    queryType("Image",    i18n("Image"));
    queryType("Audio",    i18n("Audio"));
    queryType("Video",    i18n("Video"));
    queryType("Folder",   i18n("Folder"));

    emit finishedProcessingQuery();
}

void BalooRunner::queryType(const QString &type, const QString &localizedType)
{
    Baloo::Query query;

    query.setSearchString(queryString());
    query.setType(type);
    query.setLimit(10);

    Baloo::ResultIterator it = query.exec();

    ResultList results;

    QMimeDatabase mimeDb;

    // Comment by Vishesh:
    // KRunner is absolutely retarded and allows plugins to set the global
    // relevance levels. so Baloo should not set the relevance of results too
    // high because then Applications will often appear after if the application
    // runner has not a higher relevance. So stupid.
    // Each runner plugin should not have to know about the others.
    // Anyway, that's why we're starting with .75
    float relevance = .75;
    while (it.next()) {
        Result result;

        const auto localUrl = it.filePath();
        const auto url = QUrl::fromLocalFile(localUrl);

        const auto iconName = mimeDb.mimeTypeForFile(localUrl).iconName();

        result.icon      = iconName;
        // result.resultId  = it.filePath();
        result.title     = url.fileName();
        result.url       = QUrl::fromLocalFile(it.filePath());
        result.relevance = relevance;
        relevance -= 0.05;

        auto folderPath
            = url.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash)
                  .toLocalFile();

        if (folderPath.startsWith(QDir::homePath())) {
            folderPath.replace(0, QDir::homePath().length(), QStringLiteral("~"));
        }

        result.description = folderPath;

        results << result;
    }

    emit reportNewResults(results);

}

#include "BalooRunner.moc"

