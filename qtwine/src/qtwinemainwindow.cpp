/***************************************************************************
 *   Copyright (C) 2007-2008 by George Kiagiadakis                         *
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
#include "qtwinemainwindow.h"
#include "mainwindowparts/wineconfigurationslistpart.h"
#include "mainwindowparts/wineinstallationslistpart.h"
#include "mainwindowparts/shortcutslistpart.h"
#include "dialogs/launcher.h"

#include <QMetaObject>

#include <KTabWidget>
#include <KParts/PartManager>
#include <KStandardAction>
#include <KActionCollection>
#include <KIcon>
#include <KLocalizedString>
#include <KSystemTrayIcon>
#include <KApplication>

QtWineMainWindow::QtWineMainWindow()
	: KParts::MainWindow()
{
	KSystemTrayIcon *systray = new KSystemTrayIcon(KIcon("wine"), this);
	systray->show();
	setWindowIcon(KIcon("wine"));

	m_tabWidget = new KTabWidget(this);
	m_partManager = new KParts::PartManager(this);

	//setup internal gui
	setCentralWidget(m_tabWidget);
	setupActions();
	setupGUI();

	//setup parts gui
	QMetaObject::invokeMethod(this, "loadParts", Qt::QueuedConnection);
	connect(m_partManager, SIGNAL(activePartChanged(KParts::Part*)),
		this, SLOT(createGUI(KParts::Part*)) );
}

QtWineMainWindow::~QtWineMainWindow()
{
}

void QtWineMainWindow::loadParts()
{
	ShortcutsListPart *shortcuts_part = new ShortcutsListPart(this);
	m_tabWidget->addTab(shortcuts_part->widget(), i18n("Shortcuts to installed applications"));
	m_partManager->addPart(shortcuts_part);

	WineConfigurationsListPart *configurations_part = new WineConfigurationsListPart(this);
	m_tabWidget->addTab(configurations_part->widget(), i18n("Wine Configurations"));
	m_partManager->addPart(configurations_part);
	configurations_part->widget()->hide(); // HACK I have no idea why it is needed, but it works :D

	WineInstallationsListPart *installations_part = new WineInstallationsListPart(this);
	m_tabWidget->addTab(installations_part->widget(), i18n("Wine Installations"));
	m_partManager->addPart(installations_part);
	installations_part->widget()->hide(); // HACK I have no idea why it is needed, but it works :D

	m_partManager->setActivePart(shortcuts_part);
}

void QtWineMainWindow::setupActions()
{
	KAction *installWindowsApp = new KAction(KIcon("tools-wizard"),
					i18n("Install a Windows Application"), this);
	actionCollection()->addAction("installWindowsApp", installWindowsApp);

	KAction *runCommand = new KAction(KIcon("system-run"), i18n("Run command in wine"), this);
	connect(runCommand, SIGNAL(triggered(bool)), SLOT(slotRunCommand()) );
	actionCollection()->addAction("runCommand", runCommand);

	KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
}

void QtWineMainWindow::slotRunCommand()
{
	Launcher *l = new Launcher(this);
	l->show();
	connect(l, SIGNAL(finished(int)), l, SLOT(deleteLater()) );
}

#include "qtwinemainwindow.moc"

