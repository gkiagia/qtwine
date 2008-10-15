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
#include "qtwinepreferences.h"
#include "../qtwineapplication.h"
#include "../dialogs/configurationeditor.h"
#include "../dialogs/regfilemergedialog.h"
#include "../dialogs/createconfigurationdialog.h"

#include "wineprocess.h"

#include <KDebug>
#include <KAction>
#include <KActionCollection>
#include <KIcon>
#include <KLocalizedString>
#include <KFileDialog>
#include <KRun>


WineConfigurationsListPart::WineConfigurationsListPart(QObject *parent)
    : MetaListViewPart(parent)
{
    /* define sql table fields to appear on the metabar */
    addMetabarFieldMapping(i18n("Name"), "name");
    addMetabarFieldMapping(i18n("WINEPREFIX"), "wineprefix");
    addMetabarFieldMapping(i18n("Uses installation"), "wine_installations_name");
    addMetabarFieldMapping(i18n("Description/Notes"), "description");

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
    addDefaultItemDependentAction("delete_configuration", false); //disabled when the default item is selected

    KAction *configuration_properties = new KAction(KIcon("document-properties"),
                                                    i18n("Properties"), this);
    connect(configuration_properties, SIGNAL(triggered(bool)), SLOT(configurationProperties()) );
    actionCollection()->addAction("configuration_properties", configuration_properties);
    addSelectionDependentAction("configuration_properties");

    KAction *virtual_reboot = new KAction(KIcon("system-restart"), i18n("Do a virtual reboot"), this);
    connect(virtual_reboot, SIGNAL(triggered(bool)), SLOT(virtualReboot()) );
    actionCollection()->addAction("virtual_reboot", virtual_reboot);
    addSelectionDependentAction("virtual_reboot");

    KAction *import_regfile = new KAction(KIcon("document-import"),
                                          i18n("Import a registry file..."), this);
    connect(import_regfile, SIGNAL(triggered(bool)), SLOT(importRegfile()) );
    actionCollection()->addAction("import_regfile", import_regfile);
    addSelectionDependentAction("import_regfile");

    KAction *browse_c_drive = new KAction(KIcon("document-open-folder"),
                                          i18n("Browse the virtual C: drive"), this);
    connect(browse_c_drive, SIGNAL(triggered(bool)), SLOT(browseCDrive()) );
    actionCollection()->addAction("browse_c_drive", browse_c_drive);
    addSelectionDependentAction("browse_c_drive");
        
    KAction *set_default_configuration = new KAction(KIcon("bookmark-new"),
                                                    i18n("Set this configuration as default"), this);
    actionCollection()->addAction("set_default_configuration", set_default_configuration);

#define WINELIB_ACTION(tool, icon, description) \
    KAction *run_winelib_##tool = new KAction(icon, description, this); \
    connect(run_winelib_##tool, SIGNAL(triggered(bool)), SLOT(winelib_##tool()) ); \
    actionCollection()->addAction("run_winelib_"#tool, run_winelib_##tool); \
    addSelectionDependentAction("run_winelib_"#tool);

    //tools
    WINELIB_ACTION(winecfg, KIcon("wine"), i18n("Wine's settings"))
    WINELIB_ACTION(regedit, KIcon("regedit"), i18n("Edit the registry"))
    WINELIB_ACTION(taskmgr, KIcon("utilities-system-monitor"), i18n("Open wine's task manager"))
    WINELIB_ACTION(cmd, KIcon("wcmd"), i18n("Open wine's DOS prompt"))
    WINELIB_ACTION(control, KIcon("preferences-system"), i18n("Open wine's control panel"))
    WINELIB_ACTION(uninstaller, KIcon("applications-other"), i18n("Uninstall an application"))

#undef WINELIB_ACTION
}

void WineConfigurationsListPart::loadModel()
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup group = config->group("WineConfigurationsListPart").group("MetaListViewWidget");
    setModel(qtwineApp->wineConfigurationsModel(), 1, group);
    
    /* set default item */
    int defaultId = QtWinePreferences::defaultWineConfiguration();
    int defaultRow = qtwineApp->wineConfigurationsModel()->idToRow(defaultId);

    if ( KDE_ISUNLIKELY(defaultRow == -1) )
        kWarning() << "Default item has invalid index. ID:" << defaultId << "Row:" << defaultRow;

    enableDefaultItem("set_default_configuration", KDE_ISLIKELY(defaultRow != -1) ? defaultRow : 0);
    connect(this, SIGNAL(defaultItemRowUpdated(int, int)), SLOT(saveNewDefaultItem(int)) );
}

void WineConfigurationsListPart::saveNewDefaultItem(int defaultItemRow)
{
    int id = static_cast<QtWineSqlTableModel*>(model())->rowToId(defaultItemRow);
    Q_ASSERT(id >= 0);
    QtWinePreferences::setDefaultWineConfiguration(id);
    QtWinePreferences::self()->writeConfig();
}

void WineConfigurationsListPart::createConfiguration()
{
    CreateConfigurationDialog(widget()).exec();
}

void WineConfigurationsListPart::deleteConfiguration()
{
    model()->removeRow(selectedIndex().row());
}

void WineConfigurationsListPart::configurationProperties()
{
	itemActivated(selectedIndex());
}

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

void WineConfigurationsListPart::browseCDrive()
{
    QModelIndex index = model()->index(selectedIndex().row(), model()->fieldIndex("wineprefix"));
    QString cfgPath = model()->data(index, Qt::DisplayRole).toString() + "/dosdevices/c:";
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
    WineConfiguration c = static_cast<WineConfigurationsModel*>(model())
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

#undef WINELIB_FUNCTION

#include "wineconfigurationslistpart.moc"
