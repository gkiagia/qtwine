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
#ifndef WINECONFIGURATIONSLISTPART_H
#define WINECONFIGURATIONSLISTPART_H

#include "metalistviewpart.h"

class WineConfigurationsListPart : public MetaListViewPart
{
	Q_OBJECT
public:
	WineConfigurationsListPart(QObject *parent = 0);

protected slots:
	void loadModel();
	void itemActivated(const QModelIndex & index);

private slots:
	void createConfiguration();
	void deleteConfiguration();
	void configurationProperties();
	void virtualReboot();
	void importRegfile();
	void browseCDrive();

	void winelib_winecfg();
	void winelib_regedit();
	void winelib_taskmgr();
	void winelib_cmd();
	void winelib_control();
	void winelib_uninstaller();

private:
	void runWinelibTool(const QString & name);
	void setupActions();
};

#endif
