/*
    Copyright (C) 2008  George Kiagiadakis <gkiagia@users.sourceforge.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _QTWINE_EXTENDEDQPROCESS_P_H
#define _QTWINE_EXTENDEDQPROCESS_P_H

#include "../extendedqprocess.h"

LIBQTWINE_BEGIN_NAMESPACE

class ExtendedQProcessPrivate
{
    Q_DECLARE_PUBLIC(ExtendedQProcess)
protected:
    ExtendedQProcessPrivate(ExtendedQProcess *q) : q_ptr(q) {}
    virtual ~ExtendedQProcessPrivate() {}

    virtual void _p_autoDeleteHandler();

    QString prog;
    QStringList args;
    bool autoDelete;

    ExtendedQProcess *q_ptr;
};

LIBQTWINE_END_NAMESPACE

#endif
