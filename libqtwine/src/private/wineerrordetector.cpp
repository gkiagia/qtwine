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
#include "wineerrordetector_p.h"
#include <KDebug>

LIBQTWINE_BEGIN_NAMESPACE

bool WineErrorDetector::open(QIODevice::OpenMode mode)
{
    if ( mode & QIODevice::ReadOnly ) {
        kDebug() << "WineErrorDetector does not support reading";
        return false;
    }

    return QIODevice::open(mode);
}

qint64 WineErrorDetector::readData( char*, qint64 )
{
    return -1;
}

qint64 WineErrorDetector::writeData( const char *data, qint64 maxSize )
{
    for (qint64 i=0; i<maxSize; i++) {
        switch (m_state) {
            case 0: m_state = ( data[i] == '\n' ) ? 1 : 0; break;
            case 1: m_state = ( data[i] == 'w' ) ? 2 : 0; break;
            case 2: m_state = ( data[i] == 'i' ) ? 3 : 0; break;
            case 3: m_state = ( data[i] == 'n' ) ? 4 : 0; break;
            case 4: m_state = ( data[i] == 'e' ) ? 5 : 0; break;
            case 5: m_state = ( data[i] == ':' ) ? 6 : 0; break;
            case 6:
                if ( data[i] != '\n' )
                    m_msg.append(data[i]);
                else {
                    m_lastError = m_msg;
                    m_lastError = m_lastError.trimmed();
                    kDebug() << "Wine error detected: " << m_lastError;
                    emit errorDetected(m_lastError);
                    m_msg.clear();
                    m_state = 1;
                }
                break;
            default: break;
        }
    }

    return maxSize;
}

#include "wineerrordetector_p.moc"

LIBQTWINE_END_NAMESPACE
