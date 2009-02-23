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
#ifndef PROCESSIOCONNECTOR_P_H
#define PROCESSIOCONNECTOR_P_H

#include "../libqtwine_global.h"
#include <QObject>
#include <QHash>
class QProcess;
class QIODevice;

LIBQTWINE_BEGIN_NAMESPACE

class ProcessIOConnector : public QObject
{
    Q_OBJECT
public:
    enum ProcessChannel {
        NoChannel = 0x0,
        StandardInput = 0x1,
        StandardOutput = 0x2,
        StandardError = 0x4,
        AllOutput = StandardOutput | StandardError,
        All = StandardInput | StandardOutput | StandardError
    };
    Q_DECLARE_FLAGS(ProcessChannels, ProcessChannel)

    ProcessIOConnector(QProcess *process);
    ~ProcessIOConnector();

public slots:
    void connectIODevice(QIODevice *device, ProcessChannels channels);
    void disconnectIODevice(QIODevice *device);
    void disconnectAll();

private slots:
    void inputComingFromIODevice();
    void IODeviceAboutToClose();
    void processReadStdout();
    void processReadStderr();

private:
    void writeToIODevices(const QByteArray & data, ProcessIOConnector::ProcessChannels channel);

    QProcess *m_process;
    QHash<QIODevice*, ProcessChannels> m_connectedDevices;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ProcessIOConnector::ProcessChannels)

LIBQTWINE_END_NAMESPACE

#endif
