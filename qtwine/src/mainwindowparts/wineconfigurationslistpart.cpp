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
#include "wineconfigurationslistpart.h"
#include "../qtwineapplication.h"
#include "../dialogs/configurationeditor.h"
#include "../dialogs/regfilemergedialog.h"

#include <KDebug>
#include <KAction>
#include <KActionCollection>
#include <KIcon>
#include <KLocalizedString>
#include <KFileDialog>
#include <KRun>

#include <qtwine/wineprocess.h>


WineConfigurationsListPart::WineConfigurationsListPart(QObject *parent)
	: MetaListViewPart(parent)
{
	/* define sql table fields to appear on the metabar */
	addMetabarFieldMapping(i18n("Name"), "name");
	addMetabarFieldMapping(i18n("WINEPREFIX"), "wineprefix");
	addMetabarFieldMapping(i18n("Uses installation"), "wine_installations_name");
	addMetabarFieldMapping(i18n("Description/Notes"), "description");
	addMetabarFieldMapping(i18n("Last updated with wine version"), "last_updated_wineversion");

	setupActions();
	setXMLFile("wineconfigurationslistpart.rc");
}

void WineConfigurationsListPart::itemActivated(const QModelIndex & index)
{
	ConfigurationEditor(index, widget()).exec();
}

void WineConfigurationsListPart::setupActions()
{
	KAction *create_configuration = new KAction(KIcon("list-add"),
						    i18n("Create new wine configuration"), this);
	connect(create_configuration, SIGNAL(triggered(bool)), SLOT(createConfiguration()) );
	actionCollection()->addAction("create_configuration", create_configuration);

	KAction *delete_configuration = new KAction(KIcon("edit-delete"), i18n("Delete"), this);
	connect(delete_configuration, SIGNAL(triggered(bool)), SLOT(deleteConfiguration()) );
	actionCollection()->addAction("delete_configuration", delete_configuration);
	addSelectionDependentAction("delete_configuration");
	
	KAction *configuration_properties = new KAction(KIcon("document-properties"),
			 				i18n("Properties"), this);
	connect(configuration_properties, SIGNAL(triggered(bool)), SLOT(configurationProperties()) );
	actionCollection()->addAction("configuration_properties", configuration_properties);
	addSelectionDependentAction("configuration_properties");

	KAction *update_registry = new KAction(KIcon("system-software-update"),
					       i18n("Update configuration's registry"), this);
	connect(update_registry, SIGNAL(triggered(bool)), SLOT(updateRegistry()) );
	actionCollection()->addAction("update_registry", update_registry);
	addSelectionDependentAction("update_registry");

	KAction *virtual_reboot = new KAction(KIcon("system-restart"), i18n("Do a virtual reboot"), this);
	connect(virtual_reboot, SIGNAL(triggered(bool)), SLOT(virtualReboot()) );
	actionCollection()->addAction("virtual_reboot", virtual_reboot);
	addSelectionDependentAction("virtual_reboot");

	KAction *import_regfile = new KAction(KIcon("document-import"),
					       i18n("Import a registry file..."), this);
	connect(import_regfile, SIGNAL(triggered(bool)), SLOT(importRegfile()) );
	actionCollection()->addAction("import_regfile", import_regfile);
	addSelectionDependentAction("import_regfile");

	KAction *browse_cfg_dir = new KAction(KIcon("document-open-folder"), 
					      i18n("Browse configuration's directory"), this);
	connect(browse_cfg_dir, SIGNAL(triggered(bool)), SLOT(browseConfigurationDir()) );
	actionCollection()->addAction("browse_cfg_dir", browse_cfg_dir);
	addSelectionDependentAction("browse_cfg_dir");

#define WINELIB_ACTION(tool, icon, description) \
	KAction *run_winelib_##tool = new KAction(icon, description, this); \
	connect(run_winelib_##tool, SIGNAL(triggered(bool)), SLOT(winelib_##tool()) ); \
	actionCollection()->addAction("run_winelib_"#tool, run_winelib_##tool); \
	addSelectionDependentAction("run_winelib_"#tool);

	//tools FIXME icons
	WINELIB_ACTION(winecfg, KIcon("wine"), i18n("Run wine's configuration utility"))
	WINELIB_ACTION(regedit, KIcon("wine"), i18n("Run wine's registry editor"))
	WINELIB_ACTION(taskmgr, KIcon("utilities-system-monitor"), i18n("Run wine's task manager"))
	WINELIB_ACTION(cmd, KIcon("utilities-terminal"), i18n("Run wine's DOS prompt"))
	WINELIB_ACTION(control, KIcon("preferences-system"), i18n("Run wine's control panel"))
	WINELIB_ACTION(uninstaller, KIcon("applications-other"),
		       i18n("Run wine's application uninstaller"))
	WINELIB_ACTION(oleview, KIcon("graphics-viewer-document"),
		       i18n("Run wine's OLE/COM object viewer"))
	WINELIB_ACTION(progman, KIcon("applications-other"), i18n("Run wine's program manager"))
	WINELIB_ACTION(winefile, KIcon("system-file-manager"), i18n("Run wine's file manager"))
	WINELIB_ACTION(iexplore, KIcon("internet-web-browser"), i18n("Run wine's internet explorer"))
	WINELIB_ACTION(winemine, KIcon("kmines"), i18n("Run wine's minesweeper"))
	WINELIB_ACTION(clock, KIcon("xclock"), i18n("Run wine's clock"))
	WINELIB_ACTION(notepad, KIcon("accessories-text-editor"), i18n("Run wine's notepad"))
	WINELIB_ACTION(wordpad, KIcon("accessories-text-editor"), i18n("Run wine's wordpad"))

#undef WINELIB_ACTION
}

void WineConfigurationsListPart::loadModel()
{
	setModel(qtwineApp->wineConfigurationsProvider()->model(), 1);
}

void WineConfigurationsListPart::createConfiguration()
{
}

void WineConfigurationsListPart::deleteConfiguration() {}

void WineConfigurationsListPart::configurationProperties()
{
	itemActivated(selectedIndex());
}

void WineConfigurationsListPart::updateRegistry() {}
void WineConfigurationsListPart::virtualReboot() {}

void WineConfigurationsListPart::importRegfile()
{
	KUrl regfile = KFileDialog::getOpenUrl(
				KUrl("kfiledialog:///<regfileopen>"),
				i18n("*.reg|Windows registry files (*.reg)"),
				widget(),
				i18n("Choose a registry file to merge")
			);

	if ( !regfile.url().isEmpty() ) {
		RegfileMergeDialog d(regfile, widget());
		d.setConfigurationByModelRow( selectedIndex().row() );
		d.exec();
	}
}

void WineConfigurationsListPart::browseConfigurationDir()
{
	QModelIndex index = model()->index(selectedIndex().row(), model()->fieldIndex("wineprefix"));
	QString cfgPath = model()->data(index, Qt::DisplayRole).toString();
	new KRun(KUrl(cfgPath), widget(), 0, true);
}

/*! Runs a winelib tool for the selected configuration.
 * If no configuration is selected, it displays a human
 * readable message on the status bar.
 */
void WineConfigurationsListPart::runWinelibTool(const QString & name)
{
	Q_ASSERT(selectedIndex().isValid());

	using namespace QtWine;
	WineConfiguration c = qtwineApp->wineConfigurationsProvider()
					->configurationByModelRow(selectedIndex().row());
	WineApplication a(name, c);
	if (name == "cmd")
		a.setIsConsoleApplication(true);
	WineProcess *p = new WineProcess(a);
	p->setAutoDeleteEnabled(true);
	p->start();
}

#define WINELIB_FUNCTION(tool) \
	void WineConfigurationsListPart::winelib_##tool() { runWinelibTool(#tool); }

WINELIB_FUNCTION(winecfg)
WINELIB_FUNCTION(regedit)
WINELIB_FUNCTION(taskmgr)
WINELIB_FUNCTION(cmd)
WINELIB_FUNCTION(control)
WINELIB_FUNCTION(uninstaller)
WINELIB_FUNCTION(oleview)
WINELIB_FUNCTION(progman)
WINELIB_FUNCTION(winefile)
WINELIB_FUNCTION(iexplore)
WINELIB_FUNCTION(winemine)
WINELIB_FUNCTION(clock)
WINELIB_FUNCTION(notepad)
WINELIB_FUNCTION(wordpad)

#undef WINELIB_FUNCTION

