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
#ifndef SELECTCONFIGURATIONFORNEWSHORTCUTDIALOG_H
#define SELECTCONFIGURATIONFORNEWSHORTCUTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "ui_selectConfigurationForNewShortcutDialog.h"
#include "../models/modelmanager.h"

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class SelectConfigurationForNewShortcutDialog : public QDialog
{
	Q_OBJECT
public:
	static inline quint16 select(QWidget *parent = 0);

public slots:
	inline void accept();
	inline void on_buttonBox_helpRequested();

private:
	inline SelectConfigurationForNewShortcutDialog(QWidget *parent = 0);
	Ui::selectConfigurationForNewShortcutDialog dlg;
	quint16 selectedCfgID;
};

inline SelectConfigurationForNewShortcutDialog::SelectConfigurationForNewShortcutDialog(QWidget *parent)
	: QDialog(parent)
{
	dlg.setupUi(this);
	dlg.comboBox->setModelType(ModelManager::ConfigurationsModel);
}

inline quint16 SelectConfigurationForNewShortcutDialog::select(QWidget *parent)
{
	SelectConfigurationForNewShortcutDialog d(parent);
	if ( d.exec() )
		return d.selectedCfgID;
	else
		return 0;
}

inline void SelectConfigurationForNewShortcutDialog::accept()
{
	selectedCfgID = dlg.comboBox->currentId();
	QDialog::accept();
}

inline void SelectConfigurationForNewShortcutDialog::on_buttonBox_helpRequested()
{
	QMessageBox::information(this, tr("Help"), tr("Each wine configuration contains a virtual windows drive"
			" where all the application files are saved when you install an application. To create"
			" a shortcut to an application, QtWine needs to know in which configuration its files"
			" are saved, so here you have to select in which configuration you have installed the"
			" application that you want to put in the shortcut.\n\nIf the application that you want"
			" to make a shortcut to is not installed in any configuration's drive, keep in mind that"
			" the configuration that you select here will be used when you launch the shortcut and"
			" thus, the application will run as if it was installed in there.") );
}

#endif
