/***************************************************************************
 *   Copyright (C) 2008 by George Kiagiadakis                              *
 *   gkiagia@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#include "winedebugoptions.h"
#include <QDebug>
#include <QVariant>

LIBQTWINE_BEGIN_NAMESPACE

class WineDebugOptionsPrivate
{
public:
    inline bool parseString(const QString & wineDebugStr) { s = wineDebugStr; return true; }
    inline QString toString() const { return s; }

private:
    QString s;
};


WineDebugOptions::WineDebugOptions() : d(new WineDebugOptionsPrivate) {}

WineDebugOptions::WineDebugOptions(const WineDebugOptions & other)
    : d(new WineDebugOptionsPrivate(*other.d)) {}

/*! Creates a WineDebugOptions object which initializes itself
 * by parsing the \a wineDebugStr string. \sa parseString()
 */
WineDebugOptions::WineDebugOptions(const QString & wineDebugStr)
    : d(new WineDebugOptionsPrivate)
{
    parseString(wineDebugStr);
}

/*! \overload */
WineDebugOptions::WineDebugOptions(const char * wineDebugStr)
    : d(new WineDebugOptionsPrivate)
{
    parseString(wineDebugStr);
}

WineDebugOptions::~WineDebugOptions()
{
    delete d;
}

WineDebugOptions & WineDebugOptions::operator=(const WineDebugOptions &other)
{
    delete d;
    d = new WineDebugOptionsPrivate(*other.d);
    return *this;
}

/*! Parses the string \a wineDebugStr which must be in the
 * same format that wine would expect to see in the WINEDEBUG
 * environment variable and adds the debug options listed in the string
 * to the list of debug options that this class holds.
 */
bool WineDebugOptions::parseString(const QString & wineDebugStr)
{
    return d->parseString(wineDebugStr);
}

void WineDebugOptions::enableChannel(const QString & channel, WineDebugOptions::DebugClass ch_class)
{
    Q_UNUSED(channel);
    Q_UNUSED(ch_class);
    qDebug() << Q_FUNC_INFO << "Not implemented yet";
    return;
}

void WineDebugOptions::disableChannel(const QString & channel, WineDebugOptions::DebugClass ch_class)
{
    Q_UNUSED(channel);
    Q_UNUSED(ch_class);
    qDebug() << Q_FUNC_INFO << "Not implemented yet";
    return;
}

/*! Cast operator. Casts this class to a QString
 * that lists all the debug options in the format that wine
 * expects to see in the WINEDEBUG environment variable.
 * This is used to set this environment variable before loading
 * wine.
 */
WineDebugOptions::operator QString() const
{
    return d->toString();
}

WineDebugOptions::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

LIBQTWINE_END_NAMESPACE
