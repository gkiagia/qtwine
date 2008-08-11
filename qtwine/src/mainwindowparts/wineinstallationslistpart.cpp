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
#include "wineinstallationslistpart.h"
#include "qtwinepreferences.h"
#include "../qtwineapplication.h"
#include "../dialogs/installationeditor.h"
#include "../providers/wineinstallationsprovider.h"
#include "../assistants/createinstallationassistant.h"

#include <QSqlRecord>

#include <KAction>
#include <KActionCollection>
#include <KIcon>
#include <KLocalizedString>
#include <KMessageBox>


WineInstallationsListPart::WineInstallationsListPart(QObject *parent)
    : MetaListViewPart(parent)
{
    /* define sql table fields to appear on the metabar */
    addMetabarFieldMapping(i18n("Name"), "name");
    addMetabarFieldMapping(i18n("Installation prefix"), "prefix");
    addMetabarFieldMapping(i18n("Wine loader"), "wineloader");
    addMetabarFieldMapping(i18n("Wine server"), "wineserver");
    addMetabarFieldMapping(i18n("Wine dll path"), "winedllpath");
    addMetabarFieldMapping(i18n("Wine version"), "wineversion");

    /* setup actions */
    KAction *add_installation = new KAction(KIcon("list-add"),
                                            i18n("Add new wine installation"), this);
    connect(add_installation, SIGNAL(triggered(bool)), SLOT(addInstallation()) );
    actionCollection()->addAction("add_installation", add_installation);

    KAction *remove_installation = new KAction(KIcon("list-remove"), i18n("Remove"), this);
    connect(remove_installation, SIGNAL(triggered(bool)), SLOT(removeInstallation()) );
    actionCollection()->addAction("remove_installation", remove_installation);
    addSelectionDependentAction("remove_installation");
    addDefaultItemDependentAction("remove_installation", false /*disabled when the default item is selected*/);

    KAction *installation_properties = new KAction(KIcon("document-properties"),
                                                   i18n("Properties"), this);
    connect(installation_properties, SIGNAL(triggered(bool)), SLOT(installationProperties()) );
    actionCollection()->addAction("installation_properties", installation_properties);
    addSelectionDependentAction("installation_properties");
        
    KAction *set_default_installation = new KAction(KIcon("bookmark-new"),
                                                    i18n("Set this installation as default"), this);
    actionCollection()->addAction("set_default_installation", set_default_installation);

    /* locate XMLGUI file */
    setXMLFile("wineinstallationslistpart.rc");
}

void WineInstallationsListPart::loadModel()
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup group = config->group("WineInstallationsListPart").group("MetaListViewWidget");
    setModel(installationsProvider->model(), 1, group);
    
    /* set default item */
    int defaultId = QtWinePreferences::defaultWineInstallation();
    QModelIndex index = installationsProvider->find(defaultId);

    if ( KDE_ISUNLIKELY(!index.isValid()) )
        kWarning() << "Default item has invalid index";

    enableDefaultItem("set_default_installation", index.isValid() ? index.row() : 0);
    connect(this, SIGNAL(defaultItemRowUpdated(int, int)), SLOT(saveNewDefaultItem(int)) );
}

void WineInstallationsListPart::saveNewDefaultItem(int defaultItemRow)
{
    int id = model()->record(defaultItemRow).value("id").toInt();
    Q_ASSERT(id > 0);
    QtWinePreferences::setDefaultWineInstallation(id);
    QtWinePreferences::self()->writeConfig();
}

void WineInstallationsListPart::itemActivated(const QModelIndex & index)
{
    InstallationEditor(index, widget()).exec();
}

void WineInstallationsListPart::addInstallation()
{
    CreateInstallationAssistant(widget()).exec();
}

void WineInstallationsListPart::removeInstallation()
{
    int row = selectedIndex().row();

    if ( installationsProvider->installationIsLocked(model()->record(row).value("id").toInt()) ) {
        KMessageBox::sorry(widget(), i18n("Sorry but you cannot delete this wine installation "
                                    "because it is currently used by some wine configuration.") );
        return;
    }

    model()->removeRow(row);
}

void WineInstallationsListPart::installationProperties()
{
    itemActivated(selectedIndex());
}

#include "wineinstallationslistpart.moc"
