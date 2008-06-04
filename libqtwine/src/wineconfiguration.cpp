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
#include "wineconfiguration.h"
#include <QDebug>

LIBQTWINE_BEGIN_NAMESPACE

class WineConfigurationData : public QSharedData
{
public:
	WineConfigurationData();
	WineConfigurationData(const WineConfigurationData & other);
	~WineConfigurationData();

	QString *prefix;
	WineInstallation *installation;
};

WineConfigurationData::WineConfigurationData()
	: QSharedData()
{
	prefix = NULL;
	installation = NULL;
}

WineConfigurationData::WineConfigurationData(const WineConfigurationData & other)
	: QSharedData(other)
{
	prefix = (other.prefix) ? new QString(*other.prefix) : NULL;
	installation = (other.installation) ? new WineInstallation(*other.installation) : NULL;
}

WineConfigurationData::~WineConfigurationData()
{
	delete prefix;
	delete installation;
}

WineConfiguration::WineConfiguration() : d(new WineConfigurationData) {}

WineConfiguration::WineConfiguration(const QString & winePrefix, const WineInstallation & installation)
	: d(new WineConfigurationData)
{
	setWinePrefix(winePrefix);
	setWineInstallation(installation);
}

WineConfiguration::WineConfiguration(const WineConfiguration & other) : d(other.d) {}
WineConfiguration::~WineConfiguration() {}

WineConfiguration & WineConfiguration::operator=(const WineConfiguration & other)
{
	d = other.d;
	return *this;
}

bool WineConfiguration::isInvalid() const
{
	return (d->prefix == NULL) or wineInstallation().isInvalid();
}

QString WineConfiguration::winePrefix() const
{
	return (d->prefix) ? *d->prefix : QString();
}

void WineConfiguration::setWinePrefix(const QString & winePrefix)
{
	if ( !d->prefix )
		d->prefix = new QString;
	*d->prefix = winePrefix;
}

WineInstallation WineConfiguration::wineInstallation() const
{
	return (d->installation) ? *d->installation : WineInstallation();
}

void WineConfiguration::setWineInstallation(const WineInstallation & installation)
{
	if ( !d->installation )
		d->installation = new WineInstallation;
	*d->installation = installation;
}

QDebug operator<<(QDebug dbg, const WineConfiguration & c)
{
	dbg.nospace() << "(WineConfiguration, [winePrefix: " << c.winePrefix() << "], ";
	dbg.nospace() << "[wineInstallation: " << c.wineInstallation() << "] )";

	return dbg.space();
}

LIBQTWINE_END_NAMESPACE
