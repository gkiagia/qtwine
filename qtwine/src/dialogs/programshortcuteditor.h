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
#ifndef PROGRAMSHORTCUTEDITOR_H
#define PROGRAMSHORTCUTEDITOR_H

#include <QDialog>
#include "ui_programShortcutEditor.h"

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class ProgramShortcutEditor : public QDialog
{
	Q_OBJECT
public:
	ProgramShortcutEditor(quint16 shortcutID, QWidget *parent = 0);

public slots:
	void accept();

private slots:
	void on_iconButton_clicked();
	void on_executableBrowseButton_clicked();

private:
	Ui::programShortcutEditor dlg;
	quint16 id;
	QString iconFileName;
};

#endif
