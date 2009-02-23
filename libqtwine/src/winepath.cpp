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
#include "winepath.h"
#include "winedebugoptions.h"
#include "wineprocess.h"

LIBQTWINE_BEGIN_NAMESPACE

namespace WinePath {

QString convertPath(const QString & path, PathConversionType convType,
                    const WineConfiguration & configuration)
{
    WineApplication a("winepath.exe", configuration);
    a.setWineDebugOptions("-all");

    switch (convType) {
        case UnixToWindows:
            a << "-w";
            break;
        case WindowsToUnix:
            a << "-u";
            break;
        case DosToWindows:
            a << "-l";
            break;
        case WindowsToDos:
            a << "-s";
            break;
        default:
            Q_ASSERT(false);
    }

    a << path;

    WineProcess process(a);
    process.closeReadChannel(QProcess::StandardError);
    process.execute();
    return QString(process.readAllStandardOutput()).trimmed();
}

} // end namespace WinePath

LIBQTWINE_END_NAMESPACE
