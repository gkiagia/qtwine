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
#include "wineinstallationsprovider.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>

#include <KMessage>
#include <KLocalizedString>

#include <qtwine/wineinstallation.h>
using namespace QtWine;

WineInstallationsProvider::WineInstallationsProvider(QObject *parent)
	: AbstractSqlTableProvider(parent)
{
	QSqlDatabase db = QSqlDatabase::database();
	if ( !db.tables().contains("wine_installations") )
		createFirstTimeTable();

	QSqlTableModel *model = new QSqlTableModel();
	model->setTable("wine_installations");
	setModel(model);

    updateVersionFields();
    updateDistroInstallation();
    connect(model, SIGNAL(beforeInsert(QSqlRecord&)), SLOT(model_beforeInsert(QSqlRecord&)) );
    connect(model, SIGNAL(beforeUpdate(int, QSqlRecord&)), SLOT(model_beforeUpdate(int, QSqlRecord&)) );
}

void WineInstallationsProvider::createFirstTimeTable()
{
	QSqlQuery query;
	query.exec("create table wine_installations(id int primary key,"
			" name varchar(256), prefix varchar(1024),"
			" winedllpath varchar(1024), wineloader varchar(1024),"
			" wineserver varchar(1024), wineversion varchar(10))");

	WineInstallation inst = WineInstallation::findWineInPath();
	if ( inst.isInvalid() ) {
		KMessage::message(KMessage::Warning, i18n("QtWine was unable to find an existing"
				" installation of wine. Please install wine either using your"
				" distribution's package manager (recommended) or using QtWine's"
				" assistant for installing wine.") );
	}

	query.prepare("insert into wine_installations (id, name, prefix,"
			" winedllpath, wineloader, wineserver, wineversion) "
			"values (:id, :name, :prefix, :winedllpath,"
			" :wineloader, :wineserver, :wineversion)");
	query.bindValue(":id", 1);
	query.bindValue(":name", i18n("Distribution's wine installation"));
	query.bindValue(":prefix", inst.prefix());
	query.bindValue(":winedllpath", inst.wineDllPath());
	query.bindValue(":wineloader", inst.wineLoader());
	query.bindValue(":wineserver", inst.wineServer());
	query.bindValue(":version", inst.wineVersion());
	query.exec();
}

void WineInstallationsProvider::updateVersionFields()
{
	int i, j=model()->fieldIndex("wineversion");
	QModelIndex index;
	WineInstallation inst;

	for (i=0; i < model()->rowCount(); ++i) {
		index = model()->index(i,j);
		inst = installationByModelRow(i);
		model()->setData(index, inst.wineVersion());
	}

	model()->submit();
}

void WineInstallationsProvider::updateDistroInstallation()
{
    QSqlRecord record = model()->record(0);
    if ( QFile::exists(record.value("wineloader").toString()) )
        return; //do not update if the installation still exists

    WineInstallation inst = WineInstallation::findWineInPath();
    if ( inst.isInvalid() ) {
        KMessage::message(KMessage::Warning, i18n("QtWine was unable to find an existing"
                          " installation of wine. Please install wine either using your"
                          " distribution's package manager (recommended) or using QtWine's"
                          " assistant for installing wine.") );
        return; // no reason to update, the installation is invalid either way
    }

    record.setValue("prefix", inst.prefix());
    record.setValue("winedllpath", inst.wineDllPath());
    record.setValue("wineloader", inst.wineLoader());
    record.setValue("wineserver", inst.wineServer());
    record.setValue("version", inst.wineVersion());
    model()->setRecord(0, record); //put the new information back to the model
}

WineInstallation WineInstallationsProvider::installationById(uint id) const
{
	QModelIndex index = find(id);
	if ( !index.isValid() )
		return WineInstallation();

	return installationByModelRow(index.row());
}

WineInstallation WineInstallationsProvider::installationByModelRow(int row) const
{
	QSqlRecord record = model()->record(row);
	return installationFromRecord(record);
}

WineInstallation WineInstallationsProvider::installationFromRecord(const QSqlRecord & record) const
{
	WineInstallation installation;
	installation.setPrefix(record.value("prefix").toString());
	installation.setWineLoader(record.value("wineloader").toString());
	installation.setWineServer(record.value("wineserver").toString());
	installation.setWineDllPath(record.value("winedllpath").toString());
	return installation;
}

bool WineInstallationsProvider::importInstallation(const QString & name,
						   const WineInstallation & installation)
{
	QSqlRecord record = model()->record(); // an empty record with info about the table's fields
	record.setValue("id", generateId(name));
	record.setValue("name", name);
	record.setValue("prefix", installation.prefix());
	record.setValue("wineloader", installation.wineLoader());
	record.setValue("wineserver", installation.wineServer());
	record.setValue("winedllpath", installation.wineDllPath());
	return model()->insertRecord(-1, record);
}

#if 0
/*! Removes the installation with ID \a id.
 * \return True on success, false on failure.
 * \todo Prompt the user if this is the only installation available
 * or if the installation is used by a configuration.
 */
bool WineInstallationsProvider::removeInstallation(uint id)
{
	QModelIndex index = find(id);
	if ( !index.isValid() )
		return false;

	return m_sqlModel->removeRow(index.row());
}
#endif

void WineInstallationsProvider::model_beforeInsert(QSqlRecord & record)
{
	WineInstallation i = installationFromRecord(record);
	record.setValue("wineversion", i.wineVersion());
}

void WineInstallationsProvider::model_beforeUpdate(int, QSqlRecord & record)
{
	WineInstallation i = installationFromRecord(record);
	record.setValue("wineversion", i.wineVersion());
	record.setGenerated("wineversion", true);
}

#include "wineinstallationsprovider.moc"
