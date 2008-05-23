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
#ifndef _QTWINE_REGEDIT_H
#define _QTWINE_REGEDIT_H

#include "wineconfiguration.h"

LIBQTWINE_BEGIN_NAMESPACE

/*! This namespace provides functions that offer a C++ interface
 * to the command line interface of wine's regedit utility.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
namespace RegEdit
{
	LIBQTWINE_EXPORT bool importRegFile(const QString & fileName, const WineConfiguration & wcfg);
	LIBQTWINE_EXPORT bool exportKey(const QString & key, const QString & fileName,
					const WineConfiguration & wcfg);
	LIBQTWINE_EXPORT bool deleteKey(const QString & key, const WineConfiguration & wcfg);
}

LIBQTWINE_END_NAMESPACE

#endif
