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
#ifndef CREATEINSTALLATIONWIZARD_H
#define CREATEINSTALLATIONWIZARD_H

#include <QWizard>

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class CreateInstallationWizard : public QWizard
{
	Q_OBJECT
public:
	CreateInstallationWizard(QWidget *parent = 0);
	int nextId() const;

protected:
	void initializePage(int id);

private:
	enum Pages { StartPage, ChoicePage, BasicInformationPage, AdvancedInformationPage, FinishPage };
};


namespace CreateInstallationWizardPrivate {

	class choicePage : public QWizardPage
	{
		public:
			choicePage(QWidget *parent = 0);
	};

	class basicInformationPage : public QWizardPage
	{
		Q_OBJECT

		public:
			basicInformationPage(QWidget *parent = 0);
			bool validatePage();

		public slots:
			void on_browseButton_clicked();
	};

	class advancedInformationPage : public QWizardPage
	{
		Q_OBJECT

		public:
			advancedInformationPage(QWidget *parent = 0);
			void initializePage();
			bool validatePage();

		public slots:
			void on_loaderBrowseButton_clicked();
			void on_serverBrowseButton_clicked();
			void on_dllpathBrowseButton_clicked();
	};
}

#endif
