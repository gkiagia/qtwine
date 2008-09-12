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
#include <QtGlobal>
#include <QDir>
#include <KGlobal>
#include <KStandardDirs>
#include <KRandom>
#include "../libqtwine_global.h"

LIBQTWINE_BEGIN_NAMESPACE

namespace Helpers {

/*! Generates a unique fileName for a temporary file. The fileName also contains
 * the path to it. If a KStandardDirs object is available via KGlobal, we use
 * the "tmp" resource as a temporary directory else we use QDir::tempPath().
 */
QString generateTempFileName(const QString & baseName)
{
    QString tmpDir = KGlobal::hasMainComponent() ?
            KGlobal::dirs()->saveLocation("tmp") : QDir::tempPath();
    Q_ASSERT(!tmpDir.isEmpty());

    if ( !tmpDir.endsWith('/') )
        tmpDir.append('/');

    return tmpDir.append(baseName).append(KRandom::randomString(10)).append(".tmp");
}

/*! Generates a unique socket name. On unix, this also contains a
 * path to the socket. If a KStandardDirs object is available via KGlobal, we use
 * the "socket" resource as the socket directory else we use QDir::tempPath().
 */
QString generateSocketAddress(const QString & baseName)
{
#ifdef Q_OS_UNIX
    QString tmpDir = KGlobal::hasMainComponent() ?
            KGlobal::dirs()->saveLocation("socket") : QDir::tempPath();
    Q_ASSERT(!tmpDir.isEmpty());

    if ( !tmpDir.endsWith('/') )
        tmpDir.append('/');
#else
    QString tmpDir;
#endif

    return tmpDir.append(baseName).append(KRandom::randomString(10)).append(".socket");
}

}

LIBQTWINE_END_NAMESPACE
