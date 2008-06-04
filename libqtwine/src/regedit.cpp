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
#include "regedit.h"
#include "winedebugoptions.h"
#include "wineprocess.h"

LIBQTWINE_BEGIN_NAMESPACE

namespace RegEdit {

bool importRegFile(const QString & fileName, const WineConfiguration & wcfg)
{
	WineApplication regedit("regedit.exe", wcfg);
	regedit.setWineDebugOptions("-all");
	regedit << fileName;

	WineProcess p(regedit);
	return !p.execute();
}

bool exportKey(const QString & key, const QString & fileName, const WineConfiguration & wcfg)
{
	WineApplication regedit("regedit.exe", wcfg);
	regedit.setWineDebugOptions("-all");
	regedit << "/E" << fileName << key;

	WineProcess p(regedit);
	return !p.execute();
}

bool deleteKey(const QString & key, const WineConfiguration & wcfg)
{
	WineApplication regedit("regedit.exe", wcfg);
	regedit.setWineDebugOptions("-all");
	regedit << "/D" << key;

	WineProcess p(regedit);
	return !p.execute();
}

} //end namespace RegEdit

LIBQTWINE_END_NAMESPACE
