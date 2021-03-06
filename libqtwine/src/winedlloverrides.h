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

#include <QtCore/QSharedDataPointer>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QPair>
template <class T> class QLinkedList;

LIBQTWINE_BEGIN_NAMESPACE

class WineDllOverridesData;

/*! \class WineDllOverrides winedlloverrides.h <qtwine/winedlloverrides.h>
 * \brief Represents the value of the $WINEDLLOVERRIDES environment variable.
 * \par
 * For more information about what is $WINEDLLOVERRIDES, please refer to the
 * wine manpage.
 * \par
 * There are two ways to use this class. parseString() can parse a string
 * from the format that wine parses it too and append the dll overrides that
 * this string contains to the list of overrides that this class holds. The
 * other way is to use operator<<() or addOverride() to add a dll override
 * in a more object-oriented way.
 * \par Examples:
 * \code
 * WineDllOverrides d;
 * d.parseString("comdlg32,shell32=n,b");
 * d.parseString("c:\foo\bar\baz=b");
 * \endcode
 * \code
 * WineDllOverrides o;
 * o << qMakePair("comdlg32", WineDllOverrides::FirstNativeThenBuiltin);
 * o << qMakePair("shell32", WineDllOverrides::FirstNativeThenBuiltin);
 * o.addOverride("c:\foo\bar\baz", WineDllOverrides::Builtin);
 * \endcode
 */
class LIBQTWINE_EXPORT WineDllOverrides
{
public:
    enum DllOverrideType {
        Disabled, //!< Defines that the requested dll should be disabled.
        /*! Defines that wine should only load the native
         * version of the requested dll. */
        Native,
        /*! Defines that wine should only load the built-in
         * version of the requested dll. */
        Builtin,
        /*! Defines that wine should try first to
         * load the native version of the requested dll
         * and if that fails, it should try the built-in one. */
        FirstNativeThenBuiltin,
        /*! Defines that wine should try first to
         * load the built-in version of the requested dll
         * and if that fails, it should try the native one. */
        FirstBuiltinThenNative
    };

    /*! Constructs an empty WineDllOverrides object. */
    WineDllOverrides();

    /*! Creates a new WineDllOverrides object which initializes
     * itself by parsing the \a dllOverridesStr string.
     * \sa parseString()
     */
    WineDllOverrides(const QString & dllOverridesStr);

    /*! \overload */
    WineDllOverrides(const char * dllOverridesStr);

    /*! Constructs a WineDllOverrides object that is a copy of \a other. */
    WineDllOverrides(const WineDllOverrides & other);

    /*! Destroys this WineDllOverrides object. */
    ~WineDllOverrides();


    /*! Returns true if this object holds no dll overrides and false otherwise */
    bool isEmpty() const;

    /*! Parses the string \a dllOverridesStr which must be in the
     * same format that wine would expect to see in the WINEDLLOVERRIDES
     * environment variable and adds the overrides listed in the string
     * to the list of overrides that this class holds.
     * \returns true if the override was parsed and added successfully
     * or if \a dllOverridesStr is empty and false otherwise
     */
    bool parseString(const QString & dllOverridesStr);

    /*! Adds an override for the library \a dll to be loaded in the order
     * that \a type defines. This is equivalent to operator<<().
     * \note if \a dll is an empty string, this function does nothing.
     */
    void addOverride(const QString & dll, DllOverrideType type);

    /*! Returns a list of all the \<dll name - override type\> pairs that are
     * stored internally in this class. The return type is a QLinkedList
     * as this class internally uses a QLinkedList to store the dll overrides,
     * so this is just a shallow copy of WineDllOverride's internal list.
     * In most cases, it is only needed to iterate through all the pairs, so
     * QLinkedList is the right class to use.
     */
    QLinkedList< QPair<QString, DllOverrideType> > overridesList() const;


    /*! Cast operator. Casts this class to a QString
     * that lists all the overrides in the format that wine
     * expects to see in the WINEDLLOVERRIDES environment variable.
     * This is used to set this environment variable before loading
     * wine from the WineProcess class.
     */
    operator QString() const;

    /*! Casts this object to a QVariant that encapsulates this object */
    operator QVariant() const;

    /*! Adds a dll override to the list of overrides that this class holds.
     * This is equivalent to addOverride().
     * \param p this is a pair of a string and a DllOverrideType;
     * the string is the dll name and the DllOverrideType defines its load order.
     */
    inline WineDllOverrides & operator<<(const QPair<QString, DllOverrideType> & p);

    /*! Makes this WineDllOverrides object a copy of \a other and
     * returns a reference to this object.
     */
    WineDllOverrides & operator=(const WineDllOverrides & other);

private:
    QSharedDataPointer<WineDllOverridesData> d;
};

inline WineDllOverrides & WineDllOverrides::operator<<(const QPair<QString, DllOverrideType> & p)
{
    addOverride(p.first, p.second);
    return *this;
}

LIBQTWINE_END_NAMESPACE

Q_DECLARE_METATYPE(LIBQTWINE_PREPEND_NAMESPACE(WineDllOverrides))

#endif
