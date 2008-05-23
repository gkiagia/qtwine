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
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include "commonpages.h"

LabelPage::LabelPage(QWidget *parent, const QString & labelFieldName)
	: QWizardPage(parent)
{
	label = new QLabel(this);
	label->setWordWrap(true);
	QGridLayout *glayout = new QGridLayout;
	glayout->addWidget(label);
	setLayout(glayout);

	if ( !labelFieldName.isEmpty() )
		registerField(labelFieldName, label, "text");
}

void LabelPage::setText(const QString & text)
{
	label->setText(text);
}

ProgressDialogPage::ProgressDialogPage(QWidget *parent)
	: QWizardPage(parent)
{
	progressDialog = new QProgressDialog(this);
	progressDialog->setWindowFlags(Qt::Widget); //cruel hack! downgrade the dialog :P
	progressDialog->setAutoClose(false);
	progressDialog->setAutoReset(false);

	//another hack! disable the cancel button :P
	QPushButton *nullButton = new QPushButton;
	progressDialog->setCancelButton(nullButton);
	nullButton->hide();
	
	QGridLayout *l = new QGridLayout;
	l->addWidget(progressDialog);
	setLayout(l);
}
