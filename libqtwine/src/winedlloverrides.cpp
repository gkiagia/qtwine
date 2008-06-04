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

LIBQTWINE_BEGIN_NAMESPACE

class WineDllOverridesPrivate
{
public:
	inline bool parseString(const QString & dllOverridesStr) { s = dllOverridesStr; return true; }
	inline QString toString() const { return s; }

private:
	QString s;
};


WineDllOverrides::WineDllOverrides() : d(new WineDllOverridesPrivate) {}
WineDllOverrides::WineDllOverrides(const WineDllOverrides & other)
	: d(new WineDllOverridesPrivate(*other.d)) {}
WineDllOverrides::~WineDllOverrides() { delete d; }

WineDllOverrides & WineDllOverrides::operator=(const WineDllOverrides & other)
{
	delete d;
	d = new WineDllOverridesPrivate(*other.d);
	return *this;
}

WineDllOverrides::WineDllOverrides(const QString & dllOverridesStr)
	: d(new WineDllOverridesPrivate)
{
	parseString(dllOverridesStr);
}

WineDllOverrides::WineDllOverrides(const char * dllOverridesStr)
	: d(new WineDllOverridesPrivate)
{
	parseString(dllOverridesStr);
}

bool WineDllOverrides::parseString(const QString & dllOverridesStr)
{
	return d->parseString(dllOverridesStr);
}

void WineDllOverrides::addOverride(const QString & dll, DllOverrideType typ)
{
	Q_UNUSED(dll);
	Q_UNUSED(type);
	kDebug() << "Not implemented yet";
	return;
}

WineDllOverrides::operator QString() const
{
	return d->toString();
}

LIBQTWINE_END_NAMESPACE
