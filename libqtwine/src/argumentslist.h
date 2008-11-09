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
#ifndef ARGUMENTSLIST_H
#define ARGUMENTSLIST_H

#include "libqtwine_export.h"
#include "libqtwine_global.h"
#include <QtCore/QStringList>

LIBQTWINE_BEGIN_NAMESPACE

class WineConfiguration;

class LIBQTWINE_EXPORT ArgumentsList : public QStringList
{
public:
    ArgumentsList();
    ArgumentsList(const QString & arguments);
    ArgumentsList(const QStringList & arguments);

    QString toSingleString() const;
    static QString toSingleString(const QStringList & arguments);

    void addArgumentsFromString(const QString & arguments);
    static ArgumentsList fromSingleString(const QString & arguments);

    void convertPathsToWindows(const WineConfiguration & configuration);
};

LIBQTWINE_END_NAMESPACE

#endif
