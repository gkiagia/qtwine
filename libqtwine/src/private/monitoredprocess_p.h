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
#ifndef MONITOREDPROCESS_P_H
#define MONITOREDPROCESS_P_H

#include "../monitoredprocess.h"
#include <QPointer>
#include "processioconnector_p.h"
class QFile;

LIBQTWINE_BEGIN_NAMESPACE

class MonitoredProcessPrivate
{
    Q_DECLARE_PUBLIC(MonitoredProcess)
public:
    MonitoredProcessPrivate(MonitoredProcess *qq)
        : m_connector(NULL), m_autoDelete(false), q_ptr(qq) {}
    virtual ~MonitoredProcessPrivate() {}

    void _p_autoDeleteHandler();

    QPointer<QIODevice> m_terminalDevice;
    ProcessIOConnector *m_connector;
    QFile *m_logFile[3];
    bool m_autoDelete;

protected:
    MonitoredProcess *const q_ptr;
};

LIBQTWINE_END_NAMESPACE

#endif
