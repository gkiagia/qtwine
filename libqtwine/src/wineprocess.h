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
#ifndef _QTWINE_WINEPROCESS_H
#define _QTWINE_WINEPROCESS_H

#include "wineapplication.h"
#include "monitoredprocess.h"

LIBQTWINE_BEGIN_NAMESPACE

class WineProcessPrivate;

/*! This class extends KProcess by allowing it to execute
 * directly a WineApplication object and also extends
 * KProcess::setEnv() by allowing it to manage environment lists
 * like $PATH.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT WineProcess : public MonitoredProcess
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WineProcess)
public:
    explicit WineProcess(QObject *parent = 0);
    explicit WineProcess(const WineApplication & app, QObject *parent = 0);
    virtual ~WineProcess();

    bool wineHasError() const;
    QString lastWineError() const;

    void setWineApplication(const WineApplication & app);

public Q_SLOTS:
    virtual void startMonitored();

Q_SIGNALS:
    void wineError(const QString & errorMessage);
};

LIBQTWINE_END_NAMESPACE

#endif
