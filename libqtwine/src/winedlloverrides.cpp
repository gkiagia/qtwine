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

/*! Creates a new WineDllOverrides object which initializes
 * itself by parsing the \a dllOverridesStr string. \sa parseString()
 */
WineDllOverrides::WineDllOverrides(const QString & dllOverridesStr)
	: d(new WineDllOverridesPrivate)
{
	parseString(dllOverridesStr);
}

/*! \overload */
WineDllOverrides::WineDllOverrides(const char * dllOverridesStr)
	: d(new WineDllOverridesPrivate)
{
	parseString(dllOverridesStr);
}

/*! Parses the string \a dllOverridesStr which must be in the
 * same format that wine would expect to see in the WINEDLLOVERRIDES
 * environment variable and adds the overrides listed in the string
 * to the list of overrides that this class holds.
 */
bool WineDllOverrides::parseString(const QString & dllOverridesStr)
{
	return d->parseString(dllOverridesStr);
}

/*! Adds an override for the library \a dll to be loaded in the order
 * that \a t defines. This is the same as using operator<<().
 */
void WineDllOverrides::addOverride(const QString & dll, DllOverrideType t)
{
	Q_UNUSED(dll);
	Q_UNUSED(t);
	kDebug() << "Not implemented yet";
	return;
}

/*! Cast operator. Casts this class to a QString
 * that lists all the overrides in the format that wine
 * expects to see in the WINEDLLOVERRIDES environment variable.
 * This is used to set this environment variable before loading
 * wine.
 */
WineDllOverrides::operator QString() const
{
	return d->toString();
}

LIBQTWINE_END_NAMESPACE
