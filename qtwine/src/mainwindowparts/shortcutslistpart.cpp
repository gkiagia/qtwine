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
#include <KDebug>
#include <KAction>
#include <KActionCollection>
#include <KIcon>
#include <KLocalizedString>
#include <QListView>
#include <QModelIndex>
#include <qtwine/providers/dataprovider.h>
#include "shortcutslistpart.h"

ShortcutsListPart::ShortcutsListPart(QObject *parent)
	: DataProviderListModelPart(parent)
{
	setXMLFile("shortcutslistpart.rc");
}

void ShortcutsListPart::setupMainWidget()
{
	DataProviderListModelPart::setupMainWidget();

	//find the listView
	QListView *listView = widget()->findChild<QListView*>();
	Q_ASSERT(listView);

	//make the listView be an icon view
	listView->setGridSize(QSize(90, 90));
	listView->setWordWrap(true);
	listView->setViewMode(QListView::IconMode);
	listView->setFlow(QListView::LeftToRight);
	listView->setWrapping(true);
	listView->setResizeMode(QListView::Adjust);
}

void ShortcutsListPart::editItem(const QModelIndex & index)
{
	Q_UNUSED(index);
	kDebug() << "Not Implemented Yet" << endl;
}

void ShortcutsListPart::setupActions()
{
	KAction *create_shortcut = new KAction(KIcon("document-new"), i18n("Create new shortcut"), this);
	connect(create_shortcut, SIGNAL(triggered(bool)), SLOT(createShortcut()) );
	actionCollection()->addAction("create_shortcut", create_shortcut);

	KAction *delete_shortcut = new KAction(KIcon("edit-delete"), i18n("Delete"), this);
	connect(delete_shortcut, SIGNAL(triggered(bool)), SLOT(deleteShortcut()) );
	actionCollection()->addAction("delete_shortcut", delete_shortcut);
	addSelectionDependentAction(delete_shortcut);

	KAction *run_shortcut = new KAction(KIcon("system-run"), i18n("Run"), this);
	connect(run_shortcut, SIGNAL(triggered(bool)), SLOT(runShortcut()) );
	actionCollection()->addAction("run_shortcut", run_shortcut);
	addSelectionDependentAction(run_shortcut);

	KAction *shortcut_properties = new KAction(KIcon("document-properties"), 
						   i18n("Properties"), this);
	connect(shortcut_properties, SIGNAL(triggered(bool)), SLOT(shortcutProperties()) );
	actionCollection()->addAction("shortcut_properties", shortcut_properties);
	addSelectionDependentAction(shortcut_properties);

	//FIXME we have a custom icon with an arrow pointing on the desktop....
	KAction *place_shortcut_on_desktop = new KAction(KIcon("user-desktop"),
			 			i18n("Place on the desktop"), this);
	connect(place_shortcut_on_desktop, SIGNAL(triggered(bool)), SLOT(placeOnDesktop()) );
	actionCollection()->addAction("place_shortcut_on_desktop", place_shortcut_on_desktop);
	addSelectionDependentAction(place_shortcut_on_desktop);

	KAction *export_shortcut_to_script = new KAction(KIcon("document-export"),
						i18n("Export as a shell script"), this);
	connect(export_shortcut_to_script, SIGNAL(triggered(bool)), SLOT(exportToScript()) );
	actionCollection()->addAction("export_shortcut_to_script", export_shortcut_to_script);
	addSelectionDependentAction(export_shortcut_to_script);
}

void ShortcutsListPart::loadModel()
{
	setDataProvider(DataProvider("Shortcuts"));
}

void ShortcutsListPart::createShortcut() {}
void ShortcutsListPart::deleteShortcut() {}
void ShortcutsListPart::runShortcut() {}

void ShortcutsListPart::shortcutProperties()
{
	editItem(selectedIndex());
}

void ShortcutsListPart::placeOnDesktop() {}
void ShortcutsListPart::exportToScript() {}
