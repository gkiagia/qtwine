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
#ifndef SHORTCUTSMODEL_H
#define SHORTCUTSMODEL_H

#include "qtwinesqltablemodel.h"
namespace QtWine { class WineApplication; }

class ShortcutsModel : public QtWineSqlTableModel
{
    Q_OBJECT
public:
    explicit ShortcutsModel(QObject *parent = 0);

    QtWine::WineApplication wineApplicationByModelRow(int row) const;
    QtWine::WineApplication wineApplicationFromRecord(const QSqlRecord & record) const;

private slots:
    void slotPrimeInsert(int row, QSqlRecord & record);

private:
    void createFirstTimeTable();
};

#endif
