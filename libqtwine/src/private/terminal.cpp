/***************************************************************************
 *   Copyright (C) 2007-2008 by George Kiagiadakis                         *
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
#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include "helpers.h"
#include "../libqtwine_global.h"
#include "libqtwine_private.h"

#define HELPER_EXECUTABLE "qtwine-terminal-helper"

LIBQTWINE_BEGIN_NAMESPACE

LIBQTWINE_GLOBAL_STATIC_WITH_ARGS(QString, terminalApplication, ("xterm"))

void setDefaultTerminalApplication(const QString & str)
{
    *terminalApplication = str;
}

/*! This is the default MonitoredWineProcess::OpenTerminalFn function
 * that is used if the user has not overriden it with
 * MonitoredWineProcess::setOpenTerminalFunction().
 * By default, it opens a real terminal, starts "qtwine-terminal-helper"
 * in it and uses a socket to connect to the helper.
 * The returned QIODevice is a QLocalSocket. The helper will
 * directly print anything received on the other side of this socket
 * and will send its stdin through this socket to us.
 */
QIODevice *defaultOpenTerminalFn(const QString & title)
{
    // check if terminal helper exists in $PATH
    // (to avoid waiting for 5 secs in server->waitForNewConnection() below )
    QStringList env = QProcess::systemEnvironment();
    QString pathString = env.at( env.indexOf(QRegExp("^PATH=.*")) );
    pathString.remove(0, 5); //remove the starting "PATH="
    QStringList path = pathString.split(':', QString::SkipEmptyParts);

    bool found = false;
    foreach( QString prefix, path ) {
        if ( QFile::exists( prefix + "/"HELPER_EXECUTABLE ) ) {
            found = true;
            break;
        }
    }

    if ( LIBQTWINE_ISUNLIKELY(not found) ) {
        qCritical() << "Could not find the terminal helper executable";
        return NULL;
    }

    QProcess *terminal = new QProcess;
    QLocalServer *server = new QLocalServer(terminal);
    server->listen(Helpers::generateSocketAddress("libqtwine_terminal_socket"));

    terminal->setProcessChannelMode(QProcess::ForwardedChannels); //that's better as a default. KProcess uses that too.
    terminal->start(*terminalApplication, QStringList() << "-T" << title << "-e" <<
                                          HELPER_EXECUTABLE << server->fullServerName() );

    if ( !terminal->waitForStarted(5000) ) {
        qCritical() << "Could not start the terminal."
                 << "QProcess error message:" << terminal->errorString();
        delete server;
        delete terminal;
        return NULL;
    }

    if ( !server->waitForNewConnection(5000) ) {
        qCritical() << "Waiting for the terminal helper to connect on the socket timed out." <<
            "QLocalServer error string:" << server->errorString();
        delete server;
        delete terminal;
        return NULL;
    }

    QLocalSocket *socket = server->nextPendingConnection();
    server->close();

    // this will also delete server and socket due to QObject parent-child relationships
    QObject::connect(terminal, SIGNAL(finished(int, QProcess::ExitStatus)),
                     terminal, SLOT(deleteLater()) );
    return socket;
}

LIBQTWINE_END_NAMESPACE
