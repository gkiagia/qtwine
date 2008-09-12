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

/*! \class WineInstallation wineinstallation.h <qtwine/wineinstallation.h>
 * \brief Describes an installation of wine.
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
 * \par
 * The purpose of this class is to provide a way to describe where
 * wine is installed. It has four main properties, prefix(), wineLoader()
 * wineServer() and wineDllPath() which all four describe paths as explained
 * above. The only required path is the prefix() directory path. If the
 * other paths are not provided, the defaults paths relative to prefix()
 * are used.
 * \note This class uses implicit sharing. See the Qt documentation
 * for more details on implicit sharing.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT WineInstallation
{
public:
    /*! Constructs an invalid WineInstallation. \sa isInvalid() */
    WineInstallation();

    /*! Constructs a WineInstallation that uses \a prefix
     * as a prefix directory.
     */
    WineInstallation(const QString & prefix);

    /*! Constructs a new WineInstallation that is copy of \a other.
     * Note that this function is both fast and safe because
     * WineInstallation uses implicit sharing.
     */
    WineInstallation(const WineInstallation & other);

    /*! Destroys this WineInstallation object. */
    ~WineInstallation();


    /*! Makes \a this a copy of \a other. Note that this function is
     * both fast and safe because WineInstallation uses implicit sharing.
     */
    WineInstallation & operator=(const WineInstallation & other);


    /*! Checks if this WineInstallation is invalid. A WineInstallation
     * requires at least a prefix() to be set, so it is invalid if
     * prefix() is empty.
     * \returns true if no prefix has been set, false otherwise
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
     * returns the version of wine in the format "MAJOR.MINOR.PATCH-EXTRA"
     * It will return an empty string if wineLoader() does not exist
     * or does not have enough permissions to be executed.
     */
    QString wineVersion() const;


    /*! Returns the prefix directory of this installation. */
    QString prefix() const;

    /*! Sets the prefix directory of this installation. */
    void setPrefix(const QString & path);


    /*! Returns the path to the directory where wine stores its dll files ($WINEDLLPATH).
     * If this path has not been set with setWineDllPath(), this function
     * returns a default value based on prefix() (which is "prefix/lib/wine").
     */
    QString wineDllPath() const;

    /*! Sets the path to the directory where wine stores its dll files ($WINEDLLPATH). */
    void setWineDllPath(const QString & path);


    /*! Returns the path to the "wine" executable ($WINELOADER).
     * If this path has not been set with setWineLoader(), this function
     * returns a default value based on prefix() (which is "prefix/bin/wine").
     * \note On debian and some other distributions, sometimes wine lives in
     * /usr/lib/wine/wine.bin. So, if the file "prefix/lib/wine/wine.bin" exists,
     * this function returns this path instead of "prefix/bin/wine".
     */
    QString wineLoader() const;

    /*! Sets the path to the "wine" executable ($WINELOADER). */
    void setWineLoader(const QString & loader);


    /*! Returns the path to the "wineserver" executable ($WINESERVER).
     * If this path has not been set with setWineServer(), this function
     * returns a default value based on prefix() (which is "prefix/bin/wineserver").
     * \note On debian and some other distributions, sometimes wineserver lives in
     * /usr/lib/wine/wineserver. So, if the file "prefix/lib/wine/wineserver" exists,
     * this function returns this path instead of "prefix/bin/wineserver".
     */
    QString wineServer() const;

    /*! Sets the path to the "wineserver" executable ($WINESERVER). */
    void setWineServer(const QString & server);


    /*! Finds wine in $PATH and returns a WineInstallation object that
     * represents this installation. For example, if /usr/bin/wine is found,
     * this function will return an installation that has /usr as a prefix.
     */
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
