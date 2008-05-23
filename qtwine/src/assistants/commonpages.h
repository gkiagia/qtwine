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
#ifndef COMMONPAGES_H
#define COMMONPAGES_H

#include <QWizardPage>
#include <QProgressDialog>
class QLabel;

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LabelPage : public QWizardPage
{
	Q_OBJECT
public:
	LabelPage(QWidget *parent = 0, const QString & labelFieldName = QString());
	void setText(const QString & text);

private:
	QLabel *label;
};


/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class ProgressDialogPage : public QWizardPage
{
	Q_OBJECT
public:
	ProgressDialogPage(QWidget *parent = 0);
	QProgressDialog *progressDialog;
};

#endif
