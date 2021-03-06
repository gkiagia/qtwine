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
#ifndef WINECONFIGURATIONSMODEL_H
#define WINECONFIGURATIONSMODEL_H

#include "qtwinesqltablemodel.h"
namespace QtWine { class WineConfiguration; }

class WineConfigurationsModel : public QtWineSqlTableModel
{
public:
    WineConfigurationsModel(QObject *parent = 0);

    QtWine::WineConfiguration configurationById(int id) const;
    QtWine::WineConfiguration configurationByModelRow(int row) const;

    bool createConfiguration(const QString & name, int installationId);
    bool importConfiguration(const QString & name, const QString & wineprefix, int installationId);

    bool removeRows(int row, int count, const QModelIndex & parent);

private:
    QtWine::WineConfiguration configurationFromRecord(const QSqlRecord & record) const;
    void createFirstTimeTable();
};

#endif
