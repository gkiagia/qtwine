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
#include "../libqtwine_global.h"

LIBQTWINE_BEGIN_NAMESPACE

namespace Helpers {

static QString randomString(int num_of_letters)
{
    char *x = new char[num_of_letters+1];
    Q_CHECK_PTR(x);
    int i;
    for (i=0; i<num_of_letters; i++) {
        switch (qrand() % 3) {
            case 0: x[i] = (qrand() % 10) + 48; break; // numbers
            case 1: x[i] = (qrand() % 26) + 65; break; // capital letters
            case 2: x[i] = (qrand() % 26) + 97; break; // normal letters
        }
    }
    x[num_of_letters] = '\0';
    QString s(x);
    delete x;
    return s;
}

/*! Generates a unique fileName for a temporary file.
 * The fileName also contains the path to it.
 */
QString generateTempFileName(const QString & baseName)
{
    QString tmpDir = QDir::tempPath();
    if ( !tmpDir.endsWith('/') )
        tmpDir.append('/');

    return tmpDir.append(baseName).append(randomString(10)).append(".tmp");
}

/*! Generates a unique socket name. On unix, this also contains a
 * path to the socket.
 */
QString generateSocketAddress(const QString & baseName)
{
#ifdef Q_OS_UNIX
    QString tmpDir = QDir::tempPath();
    if ( !tmpDir.endsWith('/') )
        tmpDir.append('/');
#else
    QString tmpDir;
#endif

    return tmpDir.append(baseName).append(randomString(10)).append(".socket");
}

}

LIBQTWINE_END_NAMESPACE
