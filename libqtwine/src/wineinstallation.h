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
#ifndef _QTWINE_WINEINSTALLATION_H
#define _QTWINE_WINEINSTALLATION_H

#include "libqtwine_export.h"
#include "libqtwine_global.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QMetaType>
#include <QtCore/QString>

LIBQTWINE_BEGIN_NAMESPACE

class WineInstallationData;

/*! This class describes an installation of wine.
 * \par
 * Wine consists of several binaries and libraries and it uses
 * the standard UNIX directory structure to store them under
 * a prefix directory (which usually is "/usr" in most UNIX flavors).
 * However, wine is relocatable, which means it can be installed
 * under any prefix directory and in addition it supports three
 * environment variables that can change some default paths. These are:
 * \li $WINELOADER, the path to the "wine" loader binary.
 * \li $WINESERVER, the path to the "wineserver" binary.
 * \li $WINEDLLPATH, the path to the directory where wine stores all
 * the dll files (i.e. the implementations of the windows dll files).
 *
 * \par
 * The purpose of this class is to provide a way to describe where
 * wine is installed. It has four main properties, prefix(), wineLoader()
 * wineServer() and wineDllPath() which all four describe paths as explained
 * above. The only required path is the prefix() directory path. If the
 * other paths are not provided, the defaults paths relative to prefix()
 * are used.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT WineInstallation
{
public:
	/*! Constructs a null WineInstallation. \sa isInvalid() */
	WineInstallation();

	/*! Constructs a WineInstallation that uses \a prefix
	 * as a prefix directory.
	 */
	WineInstallation(const QString & prefix);

	WineInstallation(const WineInstallation & other);

	~WineInstallation();

	WineInstallation & operator=(const WineInstallation & other);

	/*! Checks if the WineInstallation is invalid. A WineInstallation
	 * requires at least a prefix() to be set, so it is invalid if
	 * prefix() is empty.
	 * \returns true if no prefix has been set, false otherwise.
	 * \note this does not check if there really is a wine installation
	 * under the prefix() directory.
	 */
	bool isInvalid() const;

	/*! Returns prefix() + \a element. This is provided to make life easier.
	 * It automatically handles missing directory separators between prefix()
	 * and \a element.
	 */
	QString elementPath(const QString & element) const;

	/*! Runs wineLoader() with the "--version" argument, parses and
	 * returns the version of wine in the format "MAJOR.MINOR.PATCH"
	 * It will return an empty string if wineLoader() does not exist
	 * or does not have enough permissions to be executed.
	 */
	QString wineVersion() const;

	QString prefix() const;
	void setPrefix(const QString & path);

	QString wineDllPath() const;
	void setWineDllPath(const QString & path);

	QString wineLoader() const;
	void setWineLoader(const QString & loader);

	QString wineServer() const;
	void setWineServer(const QString & server);

	static WineInstallation findWineInPath();

private:
	QSharedDataPointer<WineInstallationData> d;
};

/*! \relates WineInstallation
 * This is the QDebug stream operator for WineInstallation.
 * This allows you to use qDebug() or kDebug() to dump a WineInstallation
 * object on the terminal for debugging purposes.
 */
LIBQTWINE_EXPORT QDebug operator<<(QDebug dbg, const WineInstallation & i);

LIBQTWINE_END_NAMESPACE

Q_DECLARE_METATYPE(LIBQTWINE_PREPEND_NAMESPACE(WineInstallation))

#endif
