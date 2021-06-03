/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "ServicesRunner.h"

#include <QDebug>

#include <KService>
#include <KServiceTypeTrader>
#include <KLocalizedString>
#include <KPluginFactory>

// BLADE_EXPORT_PLUGIN(servicesrunner, ServicesRunner, "blade-plugin-services.json")

ServicesRunner::ServicesRunner(QObject *parent, const QVariantList &args)
    : AbstractRunner(parent)
{
    Q_UNUSED(args)
}

ServicesRunner::~ServicesRunner()
{
}

void ServicesRunner::cancelQuery()
{
    AbstractRunner::cancelQuery();
    m_seen.clear();
}

void ServicesRunner::query()
{
    Q_EMIT startedProcessingQuery();

    ResultList results;

    QString term = queryString();

    QSet<QString> seen;
    QString query;

    if (term.length() > 1) {
        // Search for applications which are executable and case-insensitively match the search term
        // See http://techbase.kde.org/Development/Tutorials/Services/Traders#The_KTrader_Query_Language
        // if the following is unclear to you.
        query = QStringLiteral("exist Exec and ('%1' =~ Name)").arg(term);
        const KService::List services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), query);

        for (const KService::Ptr &service: services) {
            qDebug() << service->name() << "is an exact match!" << service->storageId() << service->exec();
            if (!service->noDisplay() && service->property(QStringLiteral("NotShowIn"), QVariant::String) != "KDE") {
                Result result;
                // result.setType(Plasma::QueryMatch::ExactMatch);
                setupResult(service, result);
                result.relevance = 1;
                results << result;
                seen.insert(service->storageId());
                seen.insert(service->exec());
            }
        }
    }

    // If the term length is < 3, no real point searching the Keywords and GenericName
    if (term.length() < 3) {
        query = QStringLiteral("exist Exec and ( (exist Name and '%1' ~~ Name) or ('%1' ~~ Exec) )").arg(term);
    } else {
        // Search for applications which are executable and the term case-insensitive matches any of
        // * a substring of one of the keywords
        // * a substring of the GenericName field
        // * a substring of the Name field
        // Note that before asking for the content of e.g. Keywords and GenericName we need to ask if
        // they exist to prevent a tree evaluation error if they are not defined.
        query = QStringLiteral("exist Exec and ( (exist Keywords and '%1' ~subin Keywords) or (exist GenericName and '%1' ~~ GenericName) or (exist Name and '%1' ~~ Name) or ('%1' ~~ Exec) )").arg(term);
    }

    KService::List services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), query);
    services += KServiceTypeTrader::self()->query(QStringLiteral("KCModule"), query);

    qDebug() << "got " << services.count() << " services from " << query;
    for (const KService::Ptr &service: services) {
        if (service->noDisplay()) {
            continue;
        }

        const QString id = service->storageId();
        const QString name = service->desktopEntryName();
        const QString exec = service->exec();

        if (seen.contains(id) || seen.contains(exec)) {
            qDebug() << "already seen" << id << exec;
            continue;
        }

        qDebug() << "haven't seen" << id << "so processing now";
        seen.insert(id);
        seen.insert(exec);

        Result result;
        // result.setType(Plasma::QueryMatch::PossibleMatch);
        setupResult(service, result);
        qreal relevance(0.6);

        // If the term was < 3 chars and NOT at the beginning of the App's name or Exec, then
        // chances are the user doesn't want that app.
        if (term.length() < 3) {
            if (name.startsWith(term) || exec.startsWith(term)) {
                relevance = 0.9;
            } else {
                continue;
            }
        } else if (service->name().contains(term, Qt::CaseInsensitive)) {
            relevance = 0.8;

            if (service->name().startsWith(term, Qt::CaseInsensitive)) {
                relevance += 0.1;
            }
        } else if (service->genericName().contains(term, Qt::CaseInsensitive)) {
            relevance = 0.65;

            if (service->genericName().startsWith(term, Qt::CaseInsensitive)) {
                relevance += 0.05;
            }
        } else if (service->exec().contains(term, Qt::CaseInsensitive)) {
            relevance = 0.7;

            if (service->exec().startsWith(term, Qt::CaseInsensitive)) {
                relevance += 0.05;
            }
        }

        if (service->categories().contains(QStringLiteral("KDE")) || service->serviceTypes().contains(QStringLiteral("KCModule"))) {
            // qDebug() << "found a kde thing" << id << match.subtext() << relevance;
            if (id.startsWith(QLatin1String("kde-"))) {
                qDebug() << "old" << service->name();
                if (!service->isApplication()) {
                    // avoid showing old kcms and what not
                    continue;
                }

                // This is an older version, let's disambiguate it
                QString subtext(QStringLiteral("KDE3"));

                if (!result.description.isEmpty()) {
                    subtext.append(", " + result.description);
                }

                result.description = subtext;

            } else {
                relevance += .09;
            }
        }

        qDebug() << service->name() << "is this relevant:" << relevance;
        result.relevance = relevance;
        // if (service->serviceTypes().contains(QStringLiteral("KCModule"))) {
        //     result.matchCategory = i18n("System Settings");
        // }
        results << result;
    }

    //search for applications whose categories contains the query
    query = QStringLiteral("exist Exec and (exist Categories and '%1' ~subin Categories)").arg(term);
    services = KServiceTypeTrader::self()->query(QStringLiteral("Application"), query);

    for (const KService::Ptr &service: services) {
        qDebug() << service->name() << "is an exact match!" << service->storageId() << service->exec();
        if (!service->noDisplay()) {
            QString id = service->storageId();
            QString exec = service->exec();
            if (seen.contains(id) || seen.contains(exec)) {
                qDebug() << "already seen" << id << exec;
                continue;
            }
            Result result;
            // result.setType(Plasma::QueryMatch::PossibleMatch);
            setupResult(service, result);

            qreal relevance = 0.6;
            if (service->categories().contains(QStringLiteral("X-KDE-More")) ||
                    !service->showInCurrentDesktop()) {
                relevance = 0.5;
            }

            if (service->isApplication()) {
                relevance += .04;
            }

            result.relevance = relevance;
            results << result;
        }
    }

    // search for jump list actions
    if (term.length() >= 3) {
        query = QStringLiteral("exist Actions"); // doesn't work
        services = KServiceTypeTrader::self()->query(QStringLiteral("Application"));//, query);

        for (const KService::Ptr &service: services) {
            if (service->noDisplay()) {
                continue;
            }

            for (const KServiceAction &action: service->actions()) {
                if (action.text().isEmpty() || action.exec().isEmpty() || seen.contains(action.exec())) {
                    continue;
                }

                if (!action.text().contains(term, Qt::CaseInsensitive)) {
                    continue;
                }

                Result result;
                // result.setType(Plasma::QueryMatch::HelperMatch);
                if (!action.icon().isEmpty()) {
                    result.icon = action.icon();
                } else {
                    result.icon = service->icon();
                }
                result.title = i18nc("Jump list search result, %1 is action (eg. open new tab, %2 is application (eg. browser)",
                                     "%1 - %2", action.text(), service->name());
                // result.data = action.exec();

                qreal relevance = 0.5;
                if (action.text().startsWith(term, Qt::CaseInsensitive)) {
                    relevance += 0.05;
                }

                result.relevance = relevance;

                results << result;
            }
        }
    }

    Q_EMIT reportNewResults(results);

    Q_EMIT finishedProcessingQuery();
}

void ServicesRunner::setupResult(const KService::Ptr &service, Result &result)
{
    const auto name = service->name();

    result.title    = name;
    result.url      = QUrl("application:/" + service->storageId());
    // result.data     = service->storageId();
    // result.resultId = service->storageId();

    result.description =
        (!service->genericName().isEmpty() && service->genericName() != name) ?
            service->genericName()
      : (!service->comment().isEmpty()) ?
            service->comment()
      : result.description;

    result.matchedText = result.title + " " + result.description;

    if (!service->icon().isEmpty()) {
        result.icon = service->icon();
    }
}

#include "ServicesRunner.moc"

