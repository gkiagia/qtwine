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
#include "wineprefixcreate.h"
#include "wineprocess.h"

LIBQTWINE_BEGIN_NAMESPACE

namespace WinePrefixCreate {

WineProcess *createOrUpdateAsynchronous(const WineConfiguration & wcfg)
{
	Q_ASSERT(!wcfg.isInvalid());
	WineApplication app("wineprefixcreate", wcfg);
	WineProcess *process = new WineProcess(app);

	// HACK reset the program for app
	process->clearProgram();
	process->setProgram(wcfg.wineInstallation().elementPath("bin/wineprefixcreate"));

	process->setAutoDeleteEnabled(true);
	process->start(); // TODO use startMonitored and setup a custom error detector

	return process;
}

}


LIBQTWINE_END_NAMESPACE
