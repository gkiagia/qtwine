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
#include "qtwinesqltablemodel.h"

#include <KMessage>
#include <KLocalizedString>
#include <KRandom>

#include <QHash>
#include <QSqlError>
#include <QSqlRecord>

#include <climits>

QtWineSqlTableModel(QObject *parent = 0)
    : QSqlRelationalTableModel(parent)
{
    setEditStrategy(QSqlTableModel::OnRowChange);
}

int QtWineSqlTableModel::idToRow(int id) const
{
    int column = 0;
    QModelIndex start = index(0, column);
    QModelIndexList results = match(start, Qt::DisplayRole, QVariant(QString::number(id)) );
    return (results.size() > 0) ? results.at(0).row() : -1;
}

int QtWineSqlTableModel::rowToId(int row) const
{
    if ( row < 0 or row >= rowCount() ) return -1;
    return record(row).value("id").toInt();
}

QSqlRecord QtWineSqlTableModel::recordById(int id) const
{
    return record(idToRow(id));
}

int QtWineSqlTableModel::generateId(const QString & name) const
{
    uint unsigned_id = qHash(name);

    //We take out the MS bit so that the id is always in the range 0 <= id <= INT_MAX
    int id = unsigned_id & INT_MAX; // that should take out the MS bit

    if ( id == 1 or exists(id) ) {
        do
            id = KRandom::random() & INT_MAX;
        while( id == 1 or exists(id) );
    }

    return id;
}

#include "qtwinesqltablemodel.moc"
