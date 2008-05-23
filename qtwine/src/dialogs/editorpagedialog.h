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
#ifndef EDITORPAGEDIALOG_H
#define EDITORPAGEDIALOG_H

#include <KPageDialog>

/*! This is an extension to KPageDialog that offers default buttons for editor dialogs
 * and automatic enabling/disabling of the Reset and Apply buttons. Subclasses need
 * to implement applyChanges(), which is called when either OK or Apply is clicked.
 * Note that if the user clicks OK but applyChanges returns false, the dialog won't close.
 * Subclasses also need to implement revertChanges() to reset the editor's fields to their
 * original values.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class EditorPageDialog : public KPageDialog
{
	Q_OBJECT
public:
	explicit EditorPageDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

public slots:
	void accept();

protected slots:
	void enableChangesDependentButtons();
	void disableChangesDependentButtons();

protected:
	virtual bool applyChanges() = 0;
	virtual bool revertChanges() = 0;

private slots:
	bool apply();
	bool revert();
};

#endif
