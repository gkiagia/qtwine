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
#include "qtwineapplication.h"
#include "qtwinemainwindow.h"

#include <QFile>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <KCmdLineArgs>
#include <KLocalizedString>
#include <KStandardDirs>
#include <KMessage>


QtWineApplication::QtWineApplication()
	: KUniqueApplication(), m_mainWindow(NULL)
{
	initializeDatabase();
	initializeProviders();
}

QtWineApplication::~QtWineApplication()
{
	delete m_mainWindow;
	deleteProviders();
	shutDownDatabase();
}


int QtWineApplication::newInstance()
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	int returnValue = 0;

	if ( !m_mainWindow ) {
		m_mainWindow = new QtWineMainWindow();
		if ( !args->isSet("silent") )
			m_mainWindow->show();
	} else {
		m_mainWindow->show();
		m_mainWindow->raise();
		m_mainWindow->activateWindow();
	}

exit:
	args->clear();
	return returnValue;
}

WineInstallationsProvider *QtWineApplication::wineInstallationsProvider() const
{
	return m_installationsProvider;
}

WineConfigurationsProvider *QtWineApplication::wineConfigurationsProvider() const
{
	return m_configurationsProvider;
}

void QtWineApplication::initializeDatabase()
{
	if ( !QSqlDatabase::isDriverAvailable("QSQLITE") ){
		KMessage::message(KMessage::Fatal,
				  i18n("The SQLite plugin for Qt is not installed. Please install it."));
		exit(1);
	}

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

	QString dbFile = KStandardDirs::locateLocal("data", "qtwine/qtwine.db", true);
	db.setDatabaseName(dbFile);

	if ( !db.open() ) {
		KMessage::message(KMessage::Fatal, i18n("Could not load qtwine's database"));
		exit(1);
	}
}

void QtWineApplication::initializeProviders()
{
	m_installationsProvider = new WineInstallationsProvider;
	m_configurationsProvider = new WineConfigurationsProvider;
}

void QtWineApplication::deleteProviders()
{
	delete m_installationsProvider;
}

void QtWineApplication::shutDownDatabase()
{
	QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
}

