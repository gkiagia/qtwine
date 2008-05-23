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
#include "editorpagedialog.h"

EditorPageDialog::EditorPageDialog(QWidget *parent, Qt::WindowFlags flags)
	: KPageDialog(parent, flags)
{
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply | KDialog::Reset);

	connect(this, SIGNAL(resetClicked()), SLOT(revert()) );
	connect(this, SIGNAL(applyClicked()), SLOT(apply()) );
	disableChangesDependentButtons();
}

void EditorPageDialog::accept()
{
	if (apply())
		QDialog::accept();
}

void EditorPageDialog::enableChangesDependentButtons()
{
	//FIXME apply causes strange behaviour
	// (because model reloads and invalidates even the persistent indexes)
	// TODO ask the Qt devs...
	//enableButton(KDialog::Apply, true);
	enableButton(KDialog::Reset, true);
}

void EditorPageDialog::disableChangesDependentButtons()
{
	enableButton(KDialog::Apply, false);
	enableButton(KDialog::Reset, false);
}

bool EditorPageDialog::apply()
{
	bool result = applyChanges();
	if ( result ) disableChangesDependentButtons();
	return result;
}

bool EditorPageDialog::revert()
{
	bool result = revertChanges();
	if ( result ) disableChangesDependentButtons();
	return result;
}

#include "editorpagedialog.moc"
