/*
    Copyright (C) 2008  George Kiagiadakis <gkiagia@users.sourceforge.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "argumentslist.h"
#include "winepath.h"
#include <QtConcurrentMap>
#include <QFile>
#include <QVariant>

LIBQTWINE_BEGIN_NAMESPACE

ArgumentsList::ArgumentsList() : QStringList() {}
ArgumentsList::ArgumentsList(const QStringList & arguments) : QStringList(arguments) {}

/*
    This function is copied from qprocess.cpp, which is part of the Qt toolkit.
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
    Used under the terms of the GNU General Public License version 2 or 3.
*/
static QStringList parseCombinedArgString(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i) {
        if (program.at(i) == QLatin1Char('"')) {
            ++quoteCount;
            if (quoteCount == 3) {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount) {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace()) {
            if (!tmp.isEmpty()) {
                args += tmp;
                tmp.clear();
            }
        } else {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
}

QString ArgumentsList::toSingleString() const
{
    QString result;
    QList<QString>::const_iterator it;
    for(it = constBegin(); it != constEnd(); ++it) {
        if ((*it).contains(' '))
            result += '"' + (*it) + '"';
        else
            result += (*it);
        result.append(' ');
    }
    return result;
}

//static
QString ArgumentsList::toSingleString(const QStringList & arguments)
{
    return ArgumentsList(arguments).toSingleString();
}

void ArgumentsList::addArgumentsFromString(const QString & arguments)
{
    *this << parseCombinedArgString(arguments);
}

//static
ArgumentsList ArgumentsList::fromSingleString(const QString & arguments)
{
    return ArgumentsList(parseCombinedArgString(arguments));
}


static const WineConfiguration *conversion_configuration = NULL;

static void unixToWindows2(QString & path)
{
    if ( QFile::exists(path) )
        path = WinePath::unixToWindows(path, *conversion_configuration);
}

void ArgumentsList::convertPathsToWindows(const WineConfiguration & configuration)
{
    conversion_configuration = &configuration;
    QtConcurrent::blockingMap(*this, &unixToWindows2);
    conversion_configuration = NULL;
}

ArgumentsList::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

LIBQTWINE_END_NAMESPACE
