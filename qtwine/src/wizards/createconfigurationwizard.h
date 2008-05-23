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
#ifndef CREATECONFIGURATIONWIZARD_H
#define CREATECONFIGURATIONWIZARD_H

#include <QWizard>
#include <QWizardPage>
#include <QDir>
#include "../models/modelpointer.h"
class QProgressDialog;
class ConfigModel;
class ProgressDialogPage;

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class CreateConfigurationProcess : public QObject
{
	Q_OBJECT
public:
	CreateConfigurationProcess(const QString & name, quint16 installationID,
				   QProgressDialog *pd = 0, QObject *parent = 0);

signals:
	void finished();

public slots:
	void run();
	bool success() { return allOK; }

private:
	quint16 installationID;
	QDir newConfigurationDir;
	QProgressDialog *pd;
	bool allOK;
	ModelPointer<ConfigModel> cm;
};

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class CopyConfigurationProcess : public QObject
{
	Q_OBJECT
public:
	CopyConfigurationProcess(const QString & name, quint16 configToCopyID,
				 QProgressDialog *pd = 0, QObject *parent = 0);
	
signals:
	void finished();

public slots:
	void run();
	bool success() { return allOK; }

private:
	QDir sourceDir;
	QDir newConfigurationDir;
	QProgressDialog *pd;
	bool allOK;
	ModelPointer<ConfigModel> cm;
};

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class CreateConfigurationWizard : public QWizard
{
	Q_OBJECT
public:
	CreateConfigurationWizard(QWidget *parent = 0);
	int nextId() const;

protected:
	void initializePage(int id);

private:
	enum Pages { StartPage, ChoicePage, NewConfigurationPage, CopyPage, ProgressPage, FinishPage };

	ProgressDialogPage *ppage;
};

namespace CreateConfigurationWizardPrivate {

	class choicePage : public QWizardPage
	{
		public:
			choicePage(QWidget *parent = 0);
	};

	class newConfigurationPropertiesPage : public QWizardPage
	{
		public:
			newConfigurationPropertiesPage(QWidget *parent = 0);
			bool validatePage();
	};

	class copyPropertiesPage : public QWizardPage
	{
		public:
			copyPropertiesPage(QWidget *parent = 0);
			bool validatePage();
	};
}

#endif
