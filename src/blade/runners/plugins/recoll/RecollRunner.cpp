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

#include "RecollRunner.h"

#include <QDebug>
#include <QDir>
#include <QMimeDatabase>
#include <KPluginFactory>

#include <KLocalizedString>

#include "rclconfig.h"
#include "rcldb.h"
#include "rclinit.h"
#include "rclquery.h"
#include "wasatorcl.h"
#include "docseqdb.h"

#include <memory>

#define RECOLL_MAXIMUM_SCORE .8

BLADE_EXPORT_PLUGIN(recollrunner, RecollRunner, "blade-plugin-recoll.json")

class RecollRunner::Private {
public:
    Private()
        : valid(false)
        , rclconfig(nullptr)
    {
    }

    bool valid;
    RclConfig *rclconfig;
    std::unique_ptr<Rcl::Db> rcldb;

};

RecollRunner::RecollRunner(QObject *parent, const QVariantList &args)
    : AbstractRunner(parent)
    , d(new Private())
{
    std::string reason;
    d->rclconfig = recollinit(0, 0, reason);

    if (!d->rclconfig || !d->rclconfig->ok()) {
        qWarning() << "Recoll configuration problem: " << reason.data();
        return;
    }

    // if (d->rclconfig->getDbDir().empty()) {
    //     qWarning() << "No db directory in configuration!?";
    //     return;
    // }

    d->rcldb.reset(new Rcl::Db(d->rclconfig));

    if (!d->rcldb->open(Rcl::Db::DbRO)) {
        // qWarning() << "Failed to open the database: " << d->rclconfig->getDbDir().data();
        return;
    }

    d->valid = true;
}

RecollRunner::~RecollRunner()
{
    delete d;
}

void RecollRunner::query()
{
    qDebug() << queryString();
    emit startedProcessingQuery();

    auto queryString = this->queryString().toUtf8();

    if (d->valid && queryString.length() >= 3) {

        auto query = std::make_shared<Rcl::Query>(d->rcldb.get());
        query->setCollapseDuplicates(true);

        std::string reason;
        auto sdata = std::shared_ptr<Rcl::SearchData>(
            wasaStringToRcl(d->rclconfig, "english" /*stemlang*/,
                            (const char *)queryString, reason));

        if (!query->setQuery(sdata)) {
            qWarning() << "Wrong query";
        } else {
            DocSequenceDb seq(query, "Results", sdata);

            Rcl::Doc doc;

            ResultList results;

            for (int i = 0; i < seq.getResCnt(); ++i) {
                // if (doc.pc < 50) break;

                doc.erase();
                seq.getDoc(i, doc);
                qDebug() << "########\n" << doc.url.data() << " " << doc.pc << "%";

                QString description;


                Result result;

                result.icon        = "text";
                result.url         = QString::fromUtf8(doc.url.data());
                result.title       = result.url.fileName();
                result.relevance   = doc.pc / 100.0 * RECOLL_MAXIMUM_SCORE;

                std::vector<std::string> vabs;
                seq.getAbstract(doc, vabs);

                if (vabs.size() > 0) {
                    result.description = QString::fromUtf8(vabs[0].data());

                    for (const auto &vab: vabs) {
                        result.matchedText += QString::fromUtf8(vab.data()) + " ";
                    }
                }

                results << result;

                if (i % 5 == 0) {
                    emit reportNewResults(results);
                    results.clear();
                    if (i > 20) break;
                }
            }
        }
    }

    emit finishedProcessingQuery();
}

#include "RecollRunner.moc"

