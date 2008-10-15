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
#include "wineconfigurationsmodel.h"
#include "../qtwineapplication.h"
#include "qtwinepreferences.h"

#include "wineconfiguration.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDir>

#include <KMessage>
#include <KMessageBox>
#include <KLocalizedString>
#include <KDebug>
#include <KIO/NetAccess>

using namespace QtWine;


WineConfigurationsModel::WineConfigurationsModel(QObject *parent)
    : QtWineSqlTableModel(parent)
{
    QSqlDatabase db = QSqlDatabase::database();
    if ( KDE_ISUNLIKELY(!db.tables().contains("wine_configurations")) )
        createFirstTimeTable();

    setTable("wine_configurations");
    setRelation(fieldIndex("wineinstallation"), QSqlRelation("wine_installations", "id", "name"));
    select();
}

void WineConfigurationsModel::createFirstTimeTable()
{
    kDebug() << "Creating table for the first time";

    QSqlQuery query;
    query.exec("create table wine_configurations(id int primary key,"
               " name varchar(256), wineprefix varchar(1024),"
               " description varchar(1024), wineinstallation int)");

    query.prepare("insert into wine_configurations"
                  "(id, name, wineprefix, description, wineinstallation)"
                  " values (:id, :name, :wineprefix, :description, :wineinstallation)");
    query.bindValue(":id", 1);
    query.bindValue(":name", i18n("Wine's default configuration"));
    query.bindValue(":wineprefix", QDir::homePath() + "/.wine");
    query.bindValue(":description", i18n("The default configuration that wine uses."));
    query.bindValue(":wineinstallation", 1);
    query.exec();
}

WineConfiguration WineConfigurationsModel::configurationById(int id) const
{
    int row = idToRow(id);
    if ( row == -1 )
        return WineConfiguration();

    return configurationByModelRow(row);
}

WineConfiguration WineConfigurationsModel::configurationByModelRow(int row) const
{
    // we use another model here because the QSqlRelation hides the
    // field "wineinstallation" from the original model
    QSqlQueryModel qmodel;
    qmodel.setQuery("SELECT wineprefix, wineinstallation FROM wine_configurations");
    QSqlRecord record = qmodel.record(row);
    return configurationFromRecord(record);
}

WineConfiguration WineConfigurationsModel::configurationFromRecord(const QSqlRecord & record) const
{
    WineConfiguration c;
    if ( record.isEmpty() ) return c;

    c.setWinePrefix(record.value("wineprefix").toString());

    WineInstallationsModel *m = qtwineApp->wineInstallationsModel();
    c.setWineInstallation( m->installationById(record.value("wineinstallation").toInt()) );
    return c;
}

bool WineConfigurationsModel::createConfiguration(const QString & name, int installationId)
{
    QDir cfgPrefixDir(QtWinePreferences::configurationsPrefix());
    if ( !cfgPrefixDir.mkpath(name) ) {
        KMessage::message(KMessage::Sorry, i18n("Could not create the configuration's directory. "
                            "Please verify that you have enough permissions "
                            "in this directory: %1").arg(QtWinePreferences::configurationsPrefix()) );
        return false;
    }
    return importConfiguration(name, cfgPrefixDir.filePath(name), installationId);
}

bool WineConfigurationsModel::importConfiguration(const QString & name,
                                                  const QString & wineprefix, int installationId)
{
    WineInstallationsModel *p = qtwineApp->wineInstallationsModel(); //just a shorter name :)
    int installationRow = p->idToRow(installationId);

    if ( KDE_ISUNLIKELY(installationRow == -1) ) {
        kDebug() << "invalid installation id";
        return false;
    }

    if ( KDE_ISUNLIKELY(!insertRow(rowCount())) ) {
        kDebug() << "could not insert row... that's quite impossible";
        return false;
    }

    int newRow = rowCount()-1;
    setData( index(newRow, fieldIndex("id")), generateId(name) );
    setData( index(newRow, fieldIndex("name")), name );
    setData( index(newRow, fieldIndex("wineprefix")), wineprefix );

    // set the "wineinstallation" field (using Qt::EditRole, so we insert the id)
    QModelIndex mindex =  index(newRow, fieldIndex("wine_installations_name"));
    setData( mindex, installationId, Qt::EditRole );

    // set the "wineinstallation" field (using Qt::DisplayRole, so we insert the name)
    // installationNameIndex points to the "name" field of the installation.
    QModelIndex installationNameIndex = p->index(installationRow, p->fieldIndex("name"));
    setData( mindex, installationNameIndex.data(Qt::DisplayRole), Qt::DisplayRole );

    return submit();
}

bool WineConfigurationsModel::removeRows(int row, int count, const QModelIndex & parent)
{
    while ( count != 0 ) {
        int current_row = row + count - 1;
        int current_id = rowToId(current_row);

        //find all the shorctuts refering to this wine configuration
        QSqlQuery query;
        query.prepare("SELECT id, name, wineconfiguration FROM shortcuts WHERE wineconfiguration=?");
        query.bindValue(0, current_id);
        kDebug() << query.exec();

        if ( query.next() ) { //if the query returns any results, next() will return true here
            QString msg = i18n("You are about to remove the wine configuration \"%1\". "
                            "The following shortcuts point to applications that are installed in this "
                            "wine configuration and will be also removed now:\n\n")
                            .arg( record(current_row).value("name").toString() );
            do {
                msg += QString(" - %1\n").arg(query.value(1).toString());
            } while (query.next());
            msg.append(i18n("\nAre you sure that you want to remove this wine configuration?"));
            if ( KMessageBox::questionYesNo(0, msg) == KMessageBox::No )
                return false;

            //the user really wants to remove the shortcuts, so, go on...
            ShortcutsModel *smodel = qtwineApp->programShortcutsModel();
            query.first();
            do {
                smodel->removeRow( smodel->idToRow(query.value(0).toInt()) );
            } while( query.next() );
        }

        QString winePrefix = record(current_row).value("wineprefix").toString();
        if ( !KIO::NetAccess::del(KUrl(winePrefix), 0) ) {
            KMessage::message(KMessage::Sorry, i18n("Failed to remove the directory %1").arg(winePrefix));
            return false;
        }

        if ( !QtWineSqlTableModel::removeRows(current_row, 1, parent) ) {
            kError() << "Failed to remove wine configuration from database";
            return false;
        }

        --count;
    }

    return true;
}
