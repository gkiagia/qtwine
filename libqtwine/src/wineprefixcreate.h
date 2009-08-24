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
#ifndef _QTWINE_WINEPREFIXCREATE_H
#define _QTWINE_WINEPREFIXCREATE_H

#include "wineconfiguration.h"

#if defined(QT_GUI_LIB)
# include <QtGui/QLabel>
# include <QtGui/QVBoxLayout>
# include <QtCore/QEventLoop>
# include "wineprocess.h"
#endif

LIBQTWINE_BEGIN_NAMESPACE

class WineProcess;

namespace WinePrefixCreate
{
	LIBQTWINE_EXPORT WineProcess *createOrUpdateAsynchronous(const WineConfiguration & wcfg);

	//this is inline because libqtwine does not link to QtGui. this may change in the future...
#if defined(QT_GUI_LIB)
	inline bool createOrUpdate(const WineConfiguration & wcfg, QWidget *parent = 0)
	{
		Q_ASSERT(!wcfg.isInvalid());
		QEventLoop loop;

		QWidget container(parent, Qt::Dialog);
		QVBoxLayout layout(&container);
		QLabel label(QObject::tr("Please wait while wineprefixcreate"
			" is creating or updating this wine configuration."), &container);
		label.setWordWrap(true);
		layout.addWidget(&label);
		
		WineProcess *p = createOrUpdateAsynchronous(wcfg);
		p->setAutoDeleteEnabled(false);
		QObject::connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), &loop, SLOT(quit()) );
		
		container.show();
		loop.exec();
		
		bool success = !p->exitCode();
		delete p;
		return success;
	}
#endif
}

LIBQTWINE_END_NAMESPACE

#endif
