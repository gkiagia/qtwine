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
#ifndef _QTWINE_WINEDLLOVERRIDES_H
#define _QTWINE_WINEDLLOVERRIDES_H

#include "libqtwine_export.h"
#include "libqtwine_global.h"

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QPair>

LIBQTWINE_BEGIN_NAMESPACE

class WineDllOverridesPrivate;

/*! <p> This class represents the value of the WINEDLLOVERRIDES environment
 * variable. For more information about this variable, see the wine man page. </p>
 * <p> There are two ways to use this class. parseString() can parse a string
 * from the format that wine parses it too and append the dll overrides that
 * this string contains to the list of overrides that this class holds. The
 * other way is to use operator<<() or addOverride() to add a dll override
 * in a more object-oriented way.</p>
 * <p>Examples:
 * \code
 * WineDllOverrides d;
 * d.parseString("comdlg32,shell32=n,b");
 * d.parseString("c:\foo\bar\baz=b");
 * ...
 * WineDllOverrides o;
 * o << qMakePair("comdlg32", WineDllOverrides::FirstNativeThenBuiltin);
 * o << qMakePair("shell32", WineDllOverrides::FirstNativeThenBuiltin);
 * o.addOverride("c:\foo\bar\baz", WineDllOverrides::Builtin);
 * \endcode
 * </p>
 * \todo This class is a stub. addOverride() and operator<<() do nothing
 * and parseString() simply accepts the string as it is without checking it.
 * operator QString() simply returns the string that was last passed
 * to parseString(). This behaviour will be changed in a future version
 * of this library.
 */
class LIBQTWINE_EXPORT WineDllOverrides
{
public:
	enum DllOverrideType { Disabled, Native, Builtin, FirstNativeThenBuiltin, FirstBuiltinThenNative };

	WineDllOverrides();
	WineDllOverrides(const QString & dllOverridesStr);
	WineDllOverrides(const char * dllOverridesStr);
	WineDllOverrides(const WineDllOverrides & other);
	~WineDllOverrides();

	bool parseString(const QString & dllOverridesStr);
	void addOverride(const QString & dll, DllOverrideType t);

	operator QString() const;
	inline WineDllOverrides & operator<<(QPair<QString, DllOverrideType> p);
	WineDllOverrides & operator=(const WineDllOverrides & other);

private:
	WineDllOverridesPrivate *d;
};


/*! Adds a dll override to the list of overrides that this class holds.
 * The string in the pair is the dll name and the DllOverrideType defines
 * its load order. \sa addOverride()
 */
inline WineDllOverrides & WineDllOverrides::operator<<(QPair<QString, DllOverrideType> p)
{
	addOverride(p.first, p.second);
	return *this;
}

LIBQTWINE_END_NAMESPACE

Q_DECLARE_METATYPE(LIBQTWINE_PREPEND_NAMESPACE(WineDllOverrides))

#endif
