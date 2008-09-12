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
#ifndef WINEERRORDETECTOR_H
#define WINEERRORDETECTOR_H

#include <QIODevice>
#include "../libqtwine_global.h"

LIBQTWINE_BEGIN_NAMESPACE

/*! <p> This is a simple QIODevice that connects to a WineProcess
 * and listens for wine's error messages. These messages
 * are in the form "wine: Error message". When a message is
 * caught, errorDetected() is emited. </p>
 * <p> This device is not a real I/O device. It is only an automaton
 * that detects strings in the above form. As a result, reading is
 * not supported and passing QIODevice::ReadOnly to open()
 * will return false. The only mode that makes sense is
 * QIODevice::WriteOnly. </p>
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class WineErrorDetector : public QIODevice
{
    Q_OBJECT
public:
    inline WineErrorDetector(QObject *parent = 0) : QIODevice(parent), m_state(1) {}
    bool open(QIODevice::OpenMode mode);
    QString lastError() const { return m_lastError; }

signals:
    void errorDetected(const QString & message);

protected:
    qint64 readData( char *data, qint64 maxSize );
    qint64 writeData( const char *data, qint64 maxSize );

private:
    char m_state;
    QByteArray m_msg;
    QString m_lastError;
};

LIBQTWINE_END_NAMESPACE

#endif
