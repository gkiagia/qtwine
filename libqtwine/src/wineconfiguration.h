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
#ifndef _QTWINE_WINECONFIGURATION_H
#define _QTWINE_WINECONFIGURATION_H

#include "wineinstallation.h"

LIBQTWINE_BEGIN_NAMESPACE

class WineConfigurationData;

/*! \class WineConfiguration wineconfiguration.h <qtwine/wineconfiguration.h>
 * \brief Describes a wine configuration.
 * \par
 * A wine configuration is a pair that consists of a
 * WINEPREFIX directory (see the wine manpage for details on this)
 * and a WineInstallation. This class is provided to make the API
 * of other classes and functions nicer. In fact it only stores
 * a QString (for the WINEPREFIX path) and a WineInstallation.
 * \note This class uses implicit sharing. See the Qt documentation
 * for more details on implicit sharing.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT WineConfiguration
{
public:
    /*! Constructs an invalid WineConfiguration object. \sa isInvalid() */
    WineConfiguration();

    /*! Creates a new WineConfiguration object that has set its winePrefix() field
     * to be \a winePrefix and its wineInstallation() field to be \a installation.
     */
    WineConfiguration(const QString & winePrefix, const WineInstallation & installation);

    /*! Creates a new WineConfiguration object that is copy of \a other. */
    WineConfiguration(const WineConfiguration & other);

    /*! Destroys this WineConfiguration object. */
    ~WineConfiguration();

    /*! Makes \a this a copy of \a other. \note This function is
     * both fast and safe because WineConfiguration uses implicit sharing.
     */
    WineConfiguration & operator=(const WineConfiguration & other);

    /*! Checks if this WineConfiguration is invalid. A WineConfiguration
     * requires to have both a winePrefix() and a wineInstallation() set,
     * so it is invalid if either of those two fields is invalid or empty.
     * \returns false if either winePrefix() is empty or wineInstallation()
     * is invalid, true otherwise
     * \note This does not check if the winePrefix() directory really exists.
     * \sa WineInstallation::isInvalid()
     */
    bool isInvalid() const;

    QString winePrefix() const;
    void setWinePrefix(const QString & winePrefix);

    WineInstallation wineInstallation() const;
    void setWineInstallation(const WineInstallation & installation);

    /*! Casts this object to a QVariant that encapsulates this object */
    operator QVariant() const;

private:
    QSharedDataPointer<WineConfigurationData> d;
};

/*! \relates WineConfiguration
 * This is the QDebug stream operator for WineConfiguration.
 * This allows you to use qDebug() or kDebug() to dump a WineConfiguration
 * object on the terminal for debugging purposes.
 */
LIBQTWINE_EXPORT QDebug operator<<(QDebug dbg, const WineConfiguration & c);

LIBQTWINE_END_NAMESPACE

Q_DECLARE_METATYPE(LIBQTWINE_PREPEND_NAMESPACE(WineConfiguration))

#endif
