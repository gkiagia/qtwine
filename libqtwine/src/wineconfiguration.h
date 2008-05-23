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

/*! This class describes a wine configuration.
 * A wine configuration is a pair that consists of a
 * WINEPREFIX directory (see the wine manpage for details on this)
 * and a WineInstallation.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT WineConfiguration
{
public:
	WineConfiguration();
	WineConfiguration(const QString & winePrefix, const WineInstallation & installation);
	WineConfiguration(const WineConfiguration & other);
	~WineConfiguration();

	WineConfiguration & operator=(const WineConfiguration & other);

	bool isInvalid() const;

	QString winePrefix() const;
	void setWinePrefix(const QString & winePrefix);

	WineInstallation wineInstallation() const;
	void setWineInstallation(const WineInstallation & installation);

private:
	QSharedDataPointer<WineConfigurationData> d;
};

LIBQTWINE_EXPORT QDebug operator<<(QDebug dbg, const WineConfiguration & c);

LIBQTWINE_END_NAMESPACE

Q_DECLARE_METATYPE(LIBQTWINE_PREPEND_NAMESPACE(WineConfiguration))

#endif
