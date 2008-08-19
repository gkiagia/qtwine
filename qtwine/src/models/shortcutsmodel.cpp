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
#include "shortcutsmodel.h"
#include "../qtwineapplication.h"

#include "wineapplication.h"
#include "winedebugoptions.h"
#include "winedlloverrides.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>

#include <KRandom>
#include <KDebug>

using namespace QtWine;

ShortcutsModel::ShortcutsModel(QObject *parent)
    : QtWineSqlTableModel(parent)
{
    QSqlDatabase db = QSqlDatabase::database();
    if ( !db.tables().contains("shortcuts") )
        createFirstTimeTable();

    setTable("shortcuts");
    select();
    setRelation(fieldIndex("wineconfiguration"), QSqlRelation("wine_configurations", "id", "name"));

    connect(this, SIGNAL(primeInsert(int, QSqlRecord&)), SLOT(slotPrimeInsert(int, QSqlRecord&)) );
}

void ShortcutsModel::createFirstTimeTable()
{
    kDebug() << "Creating table for the first time";

    QSqlQuery query;
    query.exec("create table shortcuts(id int primary key,"
               " name varchar(256), wineconfiguration int, icon varchar(1024),"
               " executable varchar(1024), arguments varchar(1024), workdir varchar(1024),"
               " winedlloverrides varchar(1024), winedebugoptions varchar(1024),"
               " run_in_terminal boolean, is_cui_application boolean)");
}

WineApplication ShortcutsModel::wineApplicationByModelRow(int row) const
{
    // we use another model here because the QSqlRelation hides the
    // field "wineconfiguration" from the original model
    QSqlQueryModel qmodel;
    qmodel.setQuery("SELECT wineconfiguration, executable, arguments, workdir,"
                    " winedlloverrides, winedebugoptions, is_cui_application"
                    " FROM shortcuts");
    QSqlRecord record = qmodel.record(row);
    return wineApplicationFromRecord(record);
}

WineApplication ShortcutsModel::wineApplicationFromRecord(const QSqlRecord & record) const
{
    WineApplication a;
    if ( record.isEmpty() ) return a;

    a.setApplication( record.value("executable").toString() );

    // split by spaces but exclude spaces in quotes " "
    QStringList arguments = record.value("arguments").toString().split(QRegExp("\\s(?!\"\\w+\\s\\w+\")"));
    arguments.removeAll(QString());
    a << arguments;

    a.setWorkingDirectory( record.value("workdir").toString() );
    a.setWineDllOverrides( WineDllOverrides(record.value("winedlloverrides").toString()) );
    a.setWineDebugOptions( WineDebugOptions(record.value("winedebugoptions").toString()) );
    a.setIsConsoleApplication( record.value("is_cui_application").toBool() );

    WineConfiguration c = qtwineApp->wineConfigurationsModel()
                            ->configurationById(record.value("wineconfiguration").toInt());
    a.setWinePrefix( c.winePrefix() );
    a.setWineInstallation( c.wineInstallation() );
    return a;
}

void ShortcutsModel::slotPrimeInsert(int row, QSqlRecord & record)
{
    Q_UNUSED(row);
    record.setValue("id", generateId(KRandom::randomString(5)));
    record.setValue("name", "New shortcut");
    record.setValue("wineconfiguration", 1); //FIXME I am not working :)
}

#include "shortcutsmodel.moc"
