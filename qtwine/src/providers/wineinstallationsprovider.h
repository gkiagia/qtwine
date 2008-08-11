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
#ifndef WINEINSTALLATIONSPROVIDER_H
#define WINEINSTALLATIONSPROVIDER_H

#include "abstractsqltableprovider.h"
#include <QHash>
namespace QtWine { class WineInstallation; }
class WineConfigurationsProvider;

class WineInstallationsProvider : public AbstractSqlTableProvider
{
	Q_OBJECT
public:
	WineInstallationsProvider(QObject *parent = 0);

	QtWine::WineInstallation installationById(uint id) const;
	QtWine::WineInstallation installationByModelRow(int row) const;
	QtWine::WineInstallation installationFromRecord(const QSqlRecord & record) const;

	bool importInstallation(const QString & name, const QtWine::WineInstallation & installation);
	//bool removeInstallation(uint id); // TODO maybe remove, not used

    bool installationIsLocked(int id) const;

private slots:
	void model_beforeInsert(QSqlRecord & record);
	void model_beforeUpdate(int row, QSqlRecord & record);

private:
	void createFirstTimeTable();
	void updateVersionFields();
    void updateDistroInstallation();
    
    // this is to lock installations so that they cannot be
    // deleted because they are used by some configuration
    friend class WineConfigurationsProvider;
    void lockInstallation(int id);
    void unlockInstallation(int id);
    QHash<int, int> m_lockedInstallations;
};

#endif
