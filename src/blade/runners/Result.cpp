/*
    SPDX-FileCopyrightText: 2016 Ivan Čukić <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "Result.h"

#include <QDataStream>
#include <QDebug>

void Result::serialize(QDataStream &out) const
{
    out
        << queryId

        << title
        << description
        << icon

        << url
        << relevance
        << matchedText
        << mimeType
        ;
}


void Result::deserialize(QDataStream &in)
{
    in
        >> queryId

        >> title
        >> description
        >> icon

        >> url
        >> relevance
        >> matchedText
        >> mimeType
        ;
}


QDebug &Result::debug(QDebug &out) const
{
    return out
        << "Result("
        << title
        << ")"
        ;
}

