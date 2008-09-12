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
#ifndef _QTWINE_WINEPATH_H
#define _QTWINE_WINEPATH_H

#include "wineconfiguration.h"

LIBQTWINE_BEGIN_NAMESPACE

namespace WinePath
{
    enum PathConversionType { UnixToWindows, WindowsToUnix, DosToWindows, WindowsToDos };

    LIBQTWINE_EXPORT QString convertPath(const QString & path, PathConversionType convType,
                                         const WineConfiguration & configuration);

    LIBQTWINE_EXPORT void convertWineArguments(QStringList & arguments,
                                               const WineConfiguration & configuration);


    inline QString unixToWindows(const QString & path, const WineConfiguration & configuration)
    {
        return convertPath(path, UnixToWindows, configuration);
    }

    inline QString windowsToUnix(const QString & path, const WineConfiguration & configuration)
    {
        return convertPath(path, WindowsToUnix, configuration);
    }

    inline QString dosToWindows(const QString & path, const WineConfiguration & configuration)
    {
        return convertPath(path, DosToWindows, configuration);
    }

    inline QString windowsToDos(const QString & path, const WineConfiguration & configuration)
    {
        return convertPath(path, WindowsToDos, configuration);
    }

} // end namespace WinePath

LIBQTWINE_END_NAMESPACE

#endif
