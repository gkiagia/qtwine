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
#include "wineinstallationsmodel.h"

#include "wineinstallation.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>

#include <KMessage>
#include <KLocalizedString>
#include <KDebug>

using namespace QtWine;

WineInstallationsModel::WineInstallationsModel(QObject *parent)
    : QtWineSqlTableModel(parent)
{
    QSqlDatabase db = QSqlDatabase::database();
    if ( KDE_ISUNLIKELY(!db.tables().contains("wine_installations")) )
        createFirstTimeTable();

    setTable("wine_installations");
    select();

    updateVersionFields();
    updateDistroInstallation();
    connect(this, SIGNAL(beforeInsert(QSqlRecord&)), SLOT(model_beforeInsert(QSqlRecord&)) );
    connect(this, SIGNAL(beforeUpdate(int, QSqlRecord&)), SLOT(model_beforeUpdate(int, QSqlRecord&)) );
}

void WineInstallationsModel::createFirstTimeTable()
{
    kDebug() << "Creating table for the first time";

    QSqlQuery query;
    query.exec("create table wine_installations(id int primary key,"
               " name varchar(256), prefix varchar(1024),"
               " winedllpath varchar(1024), wineloader varchar(1024),"
               " wineserver varchar(1024), wineversion varchar(10))");

    query.prepare("insert into wine_installations (id, name) values (:id, :name)");
    query.bindValue(":id", 1);
    query.bindValue(":name", i18n("Distribution's wine installation"));
    query.exec();
}

void WineInstallationsModel::updateVersionFields()
{
    int i, j=fieldIndex("wineversion");
    QModelIndex mindex;
    WineInstallation inst;

    for (i=0; i < rowCount(); ++i) {
        mindex = index(i,j);
        inst = installationByModelRow(i);
        setData(mindex, inst.wineVersion());
    }

    submit();
}

void WineInstallationsModel::updateDistroInstallation()
{
    QSqlRecord rec = record(0);
    if ( rec.isEmpty() or rec.value("id").toInt() != 1
        or QFile::exists(rec.value("wineloader").toString()) )
        return; //do not update if wineloader still exists or the installation is not the one we want

    kDebug() << "Distro installation changed. Updating";

    WineInstallation inst = WineInstallation::findWineInPath();
    if ( inst.isInvalid() ) {
        KMessage::message(KMessage::Warning, i18n("QtWine was unable to find an existing"
                          " installation of wine. Please install wine either using your"
                          " distribution's package manager (recommended) or using QtWine's"
                          " assistant for installing wine.") );
        return; // no reason to update, the installation is invalid either way
    }

    rec.setValue("prefix", inst.prefix());
    rec.setValue("winedllpath", inst.wineDllPath());
    rec.setValue("wineloader", inst.wineLoader());
    rec.setValue("wineserver", inst.wineServer());
    rec.setValue("wineversion", inst.wineVersion());
    setRecord(0, rec); //put the new information back to the model
    submit();
}

WineInstallation WineInstallationsModel::installationById(int id) const
{
    int row = idToRow(id);
    if ( row == -1 )
        return WineInstallation();

    return installationByModelRow(row);
}

WineInstallation WineInstallationsModel::installationByModelRow(int row) const
{
    QSqlRecord rec = record(row);
    return installationFromRecord(rec);
}

WineInstallation WineInstallationsModel::installationFromRecord(const QSqlRecord & record) const
{
    WineInstallation installation;
    installation.setPrefix(record.value("prefix").toString());
    installation.setWineLoader(record.value("wineloader").toString());
    installation.setWineServer(record.value("wineserver").toString());
    installation.setWineDllPath(record.value("winedllpath").toString());
    return installation;
}

bool WineInstallationsModel::importInstallation(const QString & name,
                                                const WineInstallation & installation)
{
    QSqlRecord rec = record(); // an empty record with info about the table's fields
    rec.setValue("id", generateId(name));
    rec.setValue("name", name);
    rec.setValue("prefix", installation.prefix());
    rec.setValue("wineloader", installation.wineLoader());
    rec.setValue("wineserver", installation.wineServer());
    rec.setValue("winedllpath", installation.wineDllPath());
    return insertRecord(-1, rec);
}

bool WineInstallationsModel::removeRows(int row, int count, const QModelIndex & parent)
{
    while ( count != 0 ) {
        int current_row = row + count - 1;
        if ( installationIsLocked(rowToId(current_row)) ) {
            KMessage::message(KMessage::Sorry, i18n("Sorry but you cannot delete this wine installation "
                                            "because it is currently used by some wine configuration.") );
            return false;
        }
        if ( !QtWineSqlTableModel::removeRows(current_row, 1, parent) ) return false;
        --count;
    }

    return true;
}

void WineInstallationsModel::model_beforeInsert(QSqlRecord & record)
{
    WineInstallation i = installationFromRecord(record);
    record.setValue("wineversion", i.wineVersion());
}

void WineInstallationsModel::model_beforeUpdate(int, QSqlRecord & record)
{
    WineInstallation i = installationFromRecord(record);
    record.setValue("wineversion", i.wineVersion());
    record.setGenerated("wineversion", true);
}

#include "wineinstallationsmodel.moc"
