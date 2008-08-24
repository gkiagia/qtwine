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
#include <KLocalizedString>
#include <KDebug>

using namespace QtWine;


WineConfigurationsModel::WineConfigurationsModel(QObject *parent)
    : QtWineSqlTableModel(parent)
{
    QSqlDatabase db = QSqlDatabase::database();
    if ( KDE_ISUNLIKELY(!db.tables().contains("wine_configurations")) )
        createFirstTimeTable();

    setTable("wine_configurations");
    select();
    setRelation(fieldIndex("wineinstallation"), QSqlRelation("wine_installations", "id", "name"));
        
    lockInstallations();
    connect(this, SIGNAL(beforeInsert(QSqlRecord&)), SLOT(model_beforeInsert(QSqlRecord&)) );
    connect(this, SIGNAL(beforeUpdate(int, QSqlRecord&)), SLOT(model_beforeUpdate(int, QSqlRecord&)) );
    connect(this, SIGNAL(beforeDelete(int)), SLOT(model_beforeDelete(int)) );
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


/* The following functions are responsible for locking installations so that they cannot be deleted.
This is because if an installation is deleted while a configuration is using it, the configuration
will disappear from the view of the user but remain forever in the database.
*/

void WineConfigurationsModel::lockInstallations()
{
    for (int i=0; i < rowCount(); ++i)
        qtwineApp->wineInstallationsModel()->lockInstallation(rowToId(i));
}

void WineConfigurationsModel::model_beforeInsert(QSqlRecord & record)
{
    qtwineApp->wineInstallationsModel()->lockInstallation(record.value("id").toInt());
}

void WineConfigurationsModel::model_beforeUpdate(int row, QSqlRecord & record)
{
    WineInstallationsModel *m = qtwineApp->wineInstallationsModel();
    m->unlockInstallation(rowToId(row));
    m->lockInstallation(record.value("id").toInt());
}

void WineConfigurationsModel::model_beforeDelete(int row)
{
    qtwineApp->wineInstallationsModel()->unlockInstallation(rowToId(row));
}
    
#include "wineconfigurationsmodel.moc"
