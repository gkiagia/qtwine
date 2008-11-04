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
#ifndef WINEINSTALLATIONSMODEL_H
#define WINEINSTALLATIONSMODEL_H

#include "qtwinesqltablemodel.h"
namespace QtWine { class WineInstallation; }

class WineInstallationsModel : public QtWineSqlTableModel
{
    Q_OBJECT
public:
    WineInstallationsModel(QObject *parent = 0);

    QtWine::WineInstallation installationById(int id) const;
    QtWine::WineInstallation installationByModelRow(int row) const;
    QtWine::WineInstallation installationFromRecord(const QSqlRecord & record) const;

    bool importInstallation(const QString & name, const QtWine::WineInstallation & installation);
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

private slots:
    void model_beforeInsert(QSqlRecord & record);
    void model_beforeUpdate(int row, QSqlRecord & record);

private:
    void createFirstTimeTable();
    void updateVersionFields();
    void updateDistroInstallation();
};

#endif
