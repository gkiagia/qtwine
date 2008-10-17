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
#include "winedlloverrides.h"
#include <KDebug>
#include <QLinkedList>
#include <QStringList>

LIBQTWINE_BEGIN_NAMESPACE

class WineDllOverridesData : public QSharedData
{
public:
    WineDllOverridesData() : QSharedData() {}
    WineDllOverridesData(const WineDllOverridesData & other)
        : QSharedData(other), m_overridesList(other.m_overridesList) {}

    QLinkedList< QPair<QString, WineDllOverrides::DllOverrideType> > m_overridesList;
};


WineDllOverrides::WineDllOverrides() : d(new WineDllOverridesData) {}
WineDllOverrides::WineDllOverrides(const WineDllOverrides & other) : d(other.d) {}
WineDllOverrides::~WineDllOverrides() {}

WineDllOverrides & WineDllOverrides::operator=(const WineDllOverrides & other)
{
    d = other.d;
    return *this;
}

WineDllOverrides::WineDllOverrides(const QString & dllOverridesStr)
    : d(new WineDllOverridesData)
{
    parseString(dllOverridesStr);
}

WineDllOverrides::WineDllOverrides(const char * dllOverridesStr)
    : d(new WineDllOverridesData)
{
    parseString(dllOverridesStr);
}

bool WineDllOverrides::isEmpty() const
{
    return d->m_overridesList.isEmpty();
}

bool WineDllOverrides::parseString(const QString & dllOverridesStr)
{
    if ( dllOverridesStr.isEmpty() )
        return true; //no override is ok :)

    bool returnValue = false;
    QStringList list = dllOverridesStr.split(';', QString::SkipEmptyParts);
    foreach (QString str, list) {
        QStringList parts = str.split('=');
        if ( parts.size() != 2 ) {
            kDebug() << "Skipping string" << str << "as it does not seem to be a WineDllOverride";
            continue;
        }

        QStringList dllNames = parts[0].split(',', QString::SkipEmptyParts);
        DllOverrideType type;
        if ( parts[1].isEmpty() ) type = Disabled;
        else if ( parts[1] == "n" ) type = Native;
        else if ( parts[1] == "b" ) type = Builtin;
        else if ( parts[1] == "n,b" ) type = FirstNativeThenBuiltin;
        else if ( parts[1] == "b,n" ) type = FirstBuiltinThenNative;
        else {
            kDebug() << "Could not determine override type for override:" << str;
            kDebug() << "Skipping this override...";
            continue;
        }

        foreach ( QString dll, dllNames )
            d->m_overridesList.append(qMakePair(dll, type));

        // return true if at least one override was added
        returnValue = dllNames.size() != 0 ? true : false;
    }

    return returnValue;
}

void WineDllOverrides::addOverride(const QString & dll, DllOverrideType typ)
{
    if ( !dll.isEmpty() )
        d->m_overridesList.append(qMakePair(dll, typ));
}

QLinkedList< QPair<QString, WineDllOverrides::DllOverrideType> > WineDllOverrides::overridesList() const
{
    return d->m_overridesList;
}

WineDllOverrides::operator QString() const
{
    const char *typeMap[] = { "", "n", "b", "n,b", "b,n" };
    QStringList list;
    QLinkedList< QPair<QString, DllOverrideType> >::const_iterator i = d->m_overridesList.constBegin();
    while ( i != d->m_overridesList.constEnd() ) {
        list.append( QString("%1=%2").arg((*i).first).arg(typeMap[(int)(*i).second]) );
        ++i;
    }
    return list.join(";");
}

LIBQTWINE_END_NAMESPACE
