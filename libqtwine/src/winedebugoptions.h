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
#ifndef _QTWINE_WINEDEBUGOPTIONS_H
#define _QTWINE_WINEDEBUGOPTIONS_H

#include "libqtwine_export.h"
#include "libqtwine_global.h"

#include <QtCore/QMetaType>
#include <QtCore/QString>

LIBQTWINE_BEGIN_NAMESPACE

class WineDebugOptionsPrivate;

/*! <p> This class represents the value of the WINEDEBUG environment
 * variable. For more information about this variable, see the wine man page. </p>
 * \todo This class is a stub. enableChannel() and disableChannel() do nothing
 * and parseString() simply accepts the string as it is without checking it.
 * operator QString() simply returns the string that was last passed
 * to parseString(). This behaviour will be changed in a future version
 * of this library.
 */
class LIBQTWINE_EXPORT WineDebugOptions
{
public:
    enum DebugClass { All, Err, Warn, Fixme, Trace };

    WineDebugOptions();
    WineDebugOptions(const QString & wineDebugStr);
    WineDebugOptions(const char * wineDebugStr);
    WineDebugOptions(const WineDebugOptions & other);
    ~WineDebugOptions();

    bool parseString(const QString & wineDebugStr);
    void enableChannel(const QString & channel, DebugClass ch_class = All);
    void disableChannel(const QString & channel, DebugClass ch_class = All);

    operator QString() const;
    WineDebugOptions & operator=(const WineDebugOptions &other);

private:
    WineDebugOptionsPrivate *d;
};

LIBQTWINE_END_NAMESPACE

Q_DECLARE_METATYPE(LIBQTWINE_PREPEND_NAMESPACE(WineDebugOptions))

#endif
