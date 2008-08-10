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
#include "wineconfigurationsprovider.h"
#include "../qtwineapplication.h"

#include "wineconfiguration.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlRelation>
#include <QFile>
#include <QDir>

#include <KMessage>
#include <KLocalizedString>
#include <KDebug>

using namespace QtWine;


WineConfigurationsProvider::WineConfigurationsProvider(QObject *parent)
	: AbstractSqlTableProvider(parent)
{
	QSqlDatabase db = QSqlDatabase::database();
	if ( !db.tables().contains("wine_configurations") )
		createFirstTimeTable();

	QSqlRelationalTableModel *model = new QSqlRelationalTableModel();
	model->setTable("wine_configurations");
	model->setRelation(model->fieldIndex("wineinstallation"),
			   QSqlRelation("wine_installations", "id", "name"));
	setModel(model);
}

void WineConfigurationsProvider::createFirstTimeTable()
{
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

WineConfiguration WineConfigurationsProvider::configurationById(uint id) const
{
	QModelIndex index = find(id);
	if ( !index.isValid() )
		return WineConfiguration();

	return configurationByModelRow(index.row());
}

WineConfiguration WineConfigurationsProvider::configurationByModelRow(int row) const
{
	// we use another model here because the QSqlRelation hides the
	// field "wineinstallation" from the original model
	QSqlQueryModel qmodel;
	qmodel.setQuery("SELECT wineprefix, wineinstallation FROM wine_configurations");
	QSqlRecord record = qmodel.record(row);
	return configurationFromRecord(record);
}

WineConfiguration WineConfigurationsProvider::configurationFromRecord(const QSqlRecord & record) const
{
	WineConfiguration c;
	if ( record.isEmpty() ) return c;

	c.setWinePrefix(record.value("wineprefix").toString());

	WineInstallationsProvider *p = qtwineApp->wineInstallationsProvider();
	c.setWineInstallation( p->installationById(record.value("wineinstallation").toUInt()) );
	return c;
}

bool WineConfigurationsProvider::importConfiguration(const QString & name,
					const QString & wineprefix, uint installationId)
{
	WineInstallationsProvider *p = qtwineApp->wineInstallationsProvider();
	QModelIndex installationIdIndex = p->find(installationId);

	if ( !installationIdIndex.isValid() ) {
		kDebug() << "invalid installation id";
		return false;
	}

	QSqlRelationalTableModel *m = static_cast<QSqlRelationalTableModel*>(model());
	if ( KDE_ISUNLIKELY(!m->insertRow(m->rowCount())) )
		return false;

	int newRow = m->rowCount()-1;
	m->setData( m->index(newRow, m->fieldIndex("id")), generateId(name) );
	m->setData( m->index(newRow, m->fieldIndex("name")), name );
	m->setData( m->index(newRow, m->fieldIndex("wineprefix")), wineprefix );

	// set the "wineinstallation" field (using Qt::EditRole, so we insert the id)
	QModelIndex index =  m->index(newRow, m->fieldIndex("wine_installations_name"));
	m->setData( index, installationId, Qt::EditRole );

	// set the "wineinstallation" field (using Qt::DisplayRole, so we insert the name)
	QModelIndex installationNameIndex = // points to the "name" field of the installation.
		p->model()->index(installationIdIndex.row(), p->model()->fieldIndex("name"));
	m->setData( index, installationNameIndex.data(Qt::DisplayRole), Qt::DisplayRole );

	return true;
}

