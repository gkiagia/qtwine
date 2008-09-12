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
#include "processioconnector_p.h"
#include <KProcess>
#include <QIODevice>
#include <KDebug>

LIBQTWINE_BEGIN_NAMESPACE

// PUBLIC

ProcessIOConnector::ProcessIOConnector(KProcess *process)
    : QObject(process), m_process(process)
{
    Q_ASSERT(m_process);

    m_process->setOutputChannelMode(KProcess::SeparateChannels);
    connect(m_process, SIGNAL(readyReadStandardOutput()), SLOT(processReadStdout()) );
    connect(m_process, SIGNAL(readyReadStandardError()), SLOT(processReadStderr()) );
}

ProcessIOConnector::~ProcessIOConnector()
{
    disconnectAll();
}

void ProcessIOConnector::connectIODevice(QIODevice *device, ProcessChannels channels)
{
    if ( !device->isOpen() ) {
        QIODevice::OpenMode openMode = QIODevice::NotOpen;

        if ( channels & StandardInput )
            openMode |= QIODevice::ReadOnly;

        if ( (channels & StandardOutput) || (channels & StandardError) )
            openMode |= QIODevice::WriteOnly;

        if ( !device->open(openMode) ) {
            kError() << "Cannot open QIODevice. QIODevice error message:"
                     << device->errorString();
            return;
        }
    }

    if ( channels & ProcessIOConnector::StandardInput )
        connect(device, SIGNAL(readyRead()), SLOT(inputComingFromIODevice()) );

    connect(device, SIGNAL(aboutToClose()), SLOT(IODeviceAboutToClose()) );
    m_connectedDevices.insert(device, channels);
}

void ProcessIOConnector::disconnectIODevice(QIODevice *device)
{
    Q_ASSERT(device);

    if ( m_connectedDevices.contains(device) ) {
        device->disconnect(this);
        m_connectedDevices.remove(device);
        device->close();
    } else
        kDebug() << "Cannot disconnect device. Device is not connected.";
}

void ProcessIOConnector::disconnectAll()
{
    QHashIterator<QIODevice*, ProcessChannels> it(m_connectedDevices);
    while( it.hasNext() ) {
        it.next();
        it.key()->disconnect(this);
        it.key()->close();
    }
    m_connectedDevices.clear();
}

// PRIVATE

void ProcessIOConnector::inputComingFromIODevice()
{
    if ( m_process ) {
        QIODevice *device = qobject_cast<QIODevice*>(sender());
        Q_ASSERT_X(device, Q_FUNC_INFO, "sender() is not a QIODevice");
        m_process->write( device->readAll() );
    }
}

void ProcessIOConnector::IODeviceAboutToClose()
{
    QIODevice *device = qobject_cast<QIODevice*>(sender());
    Q_ASSERT_X(device, Q_FUNC_INFO, "sender() is not a QIODevice");
    disconnectIODevice(device);
}

void ProcessIOConnector::processReadStdout()
{
    if ( !m_connectedDevices.isEmpty() ) {
        QByteArray data = m_process->readAllStandardOutput();
        writeToIODevices(data, ProcessIOConnector::StandardOutput);
    }
}

void ProcessIOConnector::processReadStderr()
{
    if ( !m_connectedDevices.isEmpty() ) {
        QByteArray data = m_process->readAllStandardError();
        writeToIODevices(data, ProcessIOConnector::StandardError);
    }
}

void ProcessIOConnector::writeToIODevices(const QByteArray & data,
                                          ProcessIOConnector::ProcessChannels channel)
{
    QHashIterator<QIODevice*, ProcessChannels> it(m_connectedDevices);
    while( it.hasNext() ) {
        it.next();
        if ( it.value() & channel )
            it.key()->write(data);
    }
}

#include "processioconnector_p.moc"

LIBQTWINE_END_NAMESPACE
