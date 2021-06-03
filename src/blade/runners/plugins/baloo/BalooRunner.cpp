/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

