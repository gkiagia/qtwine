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
#include <QMap>
#include <QStringList>

LIBQTWINE_BEGIN_NAMESPACE

class WineDllOverridesData : public QSharedData
{
public:
    WineDllOverridesData() : QSharedData() {}
    WineDllOverridesData(const WineDllOverridesData & other) : QSharedData(other), m_map(other.m_map) {}

    QMap<QString, WineDllOverrides::DllOverrideType> m_map;
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

bool WineDllOverrides::parseString(const QString & dllOverridesStr)
{
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
            d->m_map.insert(dll, type);

        // return true if at least one override was added
        returnValue = dllNames.size() != 0 ? true : false;
    }

    return returnValue;
}

void WineDllOverrides::addOverride(const QString & dll, DllOverrideType typ)
{
    d->m_map.insert(dll, typ);
}

WineDllOverrides::operator QString() const
{
    const char *typeMap[] = { "", "n", "b", "n,b", "b,n" };
    QStringList list;
    QMap<QString, DllOverrideType>::const_iterator i = d->m_map.constBegin();
    while ( i != d->m_map.constEnd() ) {
        list.append( QString("%1=%2").arg(i.key()).arg(typeMap[(int)i.value()]) );
        ++i;
    }
    return list.join(";");
}

LIBQTWINE_END_NAMESPACE
