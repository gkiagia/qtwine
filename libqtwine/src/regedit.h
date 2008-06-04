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

/*! \brief Provides functions that offer a C++ interface for using regedit.exe
 * \par
 * This namespace provides some wrapper functions that call regedit.exe
 * with the correct arguments to do a specified action.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
namespace RegEdit
{
	/*! Imports the contents of the *.reg file \a fileName into the
	 * registry of the WINEPREFIX specified by \a wcfg using the wine installation
	 * specified by \a wcfg.
	 * \note This simply calls \p regedit.exe with the correct arguments.
	 * \returns true if regedit exited with exit code 0 or false otherwise
	 */
	LIBQTWINE_EXPORT bool importRegFile(const QString & fileName, const WineConfiguration & wcfg);

	/*! Exports the fields and subkeys of the registry key specified by \a key from
	 * the registry of the WINEPREFIX specified by \a wcfg using the wine installation
	 * specified by \a wcfg. The output is saved in the file \a fileName, which will
	 * be in .reg format. If \a fileName exists, it is overwritten.
	 * \note This simply calls \p regedit.exe with the correct arguments.
	 * \returns true if regedit exited with exit code 0 or false otherwise
	 */
	LIBQTWINE_EXPORT bool exportKey(const QString & key, const QString & fileName,
					const WineConfiguration & wcfg);

	/*! Deletes the registry key \a key together with its fields and subkeys from
	 * the registry of the WINEPREFIX specified by \a wcfg using the wine installation
	 * specified by \a wcfg.
	 * \note This simply calls \p regedit.exe with the correct arguments.
	 * \returns true if regedit exited with exit code 0 or false otherwise
	 */
	LIBQTWINE_EXPORT bool deleteKey(const QString & key, const WineConfiguration & wcfg);
}

LIBQTWINE_END_NAMESPACE

#endif
