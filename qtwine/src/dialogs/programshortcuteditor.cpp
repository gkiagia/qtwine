/***************************************************************************
 *   Copyright (C) 2007 by George Kiagiadakis                              *
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
#include <QFileDialog>
#include <QImageReader>

#include <debug.h>
#include "programshortcuteditor.h"
#include "../models/modelindex.h"
#include "../macros.h"

ProgramShortcutEditor::ProgramShortcutEditor(quint16 shortcutID, QWidget *parent)
 : QDialog(parent), id(shortcutID)
{
	ModelIndex i(id, ModelManager::ShortcutsModel);
	if ( !i.isValid() ) {
		printDBG("invalid ID");
		return;
	}
	
	dlg.setupUi(this);
	dlg.iconButton->setIcon( qvariant_cast<QIcon>(i.propertyData("Icon")) );
	dlg.nameEdit->setText( i.propertyData("Name").toString() );
	dlg.executableEdit->setText( i.propertyData("Executable").toString() );
	dlg.argsEdit->setText( i.propertyData("Arguments").toString() );
	dlg.wineDllOverridesEdit->setText( i.propertyData("WineDllOverrides").toString() );
	dlg.wineDebugEdit->setText( i.propertyData("WineDebugOptions").toString() );
	dlg.terminalBox->setChecked( i.propertyData("RunInTerminal").toBool() );
	dlg.dosBox->setChecked( i.propertyData("IsDosApplication").toBool() );
}

static inline QString filterForSupportedImageTypes()
{
	QList<QByteArray> list = QImageReader::supportedImageFormats();
	QString filter;
	QString defaultFilter = QObject::tr("All images(");

	foreach(QByteArray a, list) {
		defaultFilter += QString("*.%1 ").arg(QString(a).toLower());
		filter += QObject::tr("%1 images(*.%2);;").arg(QString(a).toUpper()).arg(QString(a).toLower());
	}
	defaultFilter.chop(1); //remove the last space
	defaultFilter += ");;";
	filter.prepend(defaultFilter);
	filter.chop(2); //remove the last ;;
	return filter;
}

void ProgramShortcutEditor::on_iconButton_clicked()
{
	QString icon = QFileDialog::getOpenFileName(this, tr("Select an icon"),
			QDir::homePath(), filterForSupportedImageTypes());
	if ( !icon.isEmpty() ) {
		iconFileName = icon;
		dlg.iconButton->setIcon(QIcon(icon));
	}
}

void ProgramShortcutEditor::on_executableBrowseButton_clicked()
{
	ModelIndex i(id, ModelManager::ShortcutsModel);
	ModelIndex cfgIndex(i.propertyData("ConfigurationID").toUInt(), ModelManager::ConfigurationsModel);
	QString drive_c = cfgIndex.propertyData("Path").toString() + "/dosdevices/c:";

	QString exe = QFileDialog::getOpenFileName(this, tr("Select a windows executable"),
						   drive_c, tr("WINE executables (*.exe *.exe.so)"));

	if ( exe.isEmpty() )
		return;

	//if the executable is in C:, make the path relative to C:
	if ( exe.startsWith(drive_c) ) {
		drive_c.chop(2); //remove the trailing c:
		exe.remove(0, drive_c.size()); //remove /path/to/C:
	}

	dlg.executableEdit->setText(exe);
}

void ProgramShortcutEditor::accept()
{
	ModelIndex i(id, ModelManager::ShortcutsModel);
	if ( !i.isValid() ) {
		printDBG("invalid ID");
		return;
	}

	if ( !iconFileName.isEmpty() )
		i.setPropertyData("IconFileName", iconFileName);
	i.setPropertyData("Name", dlg.nameEdit->text() );
	i.setPropertyData("Executable", dlg.executableEdit->text() );
	i.setPropertyData("Arguments", dlg.argsEdit->text() );
	i.setPropertyData("WineDllOverrides", dlg.wineDllOverridesEdit->text() );
	i.setPropertyData("WineDebugOptions", dlg.wineDebugEdit->text() );
	i.setPropertyData("RunInTerminal", dlg.terminalBox->isChecked() );
	i.setPropertyData("IsDosApplication", dlg.dosBox->isChecked() );

	QDialog::accept();
}

