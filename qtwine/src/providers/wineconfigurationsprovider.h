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
#ifndef WINECONFIGURATIONSPROVIDER_H
#define WINECONFIGURATIONSPROVIDER_H

#include "abstractsqltableprovider.h"
namespace QtWine { class WineConfiguration; }

class WineConfigurationsProvider : public AbstractSqlTableProvider
{
    Q_OBJECT
public:
	WineConfigurationsProvider(QObject *parent = 0);

	QtWine::WineConfiguration configurationById(uint id) const;
	QtWine::WineConfiguration configurationByModelRow(int row) const;

	bool importConfiguration(const QString & name, const QString & wineprefix, uint installationId);

private slots:
    void model_beforeInsert(QSqlRecord & record);
    void model_beforeUpdate(int row, QSqlRecord & record);
    void model_beforeDelete(int row);

private:
	QtWine::WineConfiguration configurationFromRecord(const QSqlRecord & record) const;
	void createFirstTimeTable();
    void lockInstallations();
};

#endif
