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
#ifndef INSTALLAPPLICATIONWIZARD_H
#define INSTALLAPPLICATIONWIZARD_H

#include <QWizard>
class ProgressDialogPage;

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class InstallApplicationWizard : public QWizard
{
	Q_OBJECT
public:
	InstallApplicationWizard(QWidget *parent = 0);
	int nextId() const;
	void setSetupExecutable(const QString & filename);

protected:
	void initializePage(int id);

private:
	enum Pages { StartPage, LocateSetupPage, ChoicePage, NewConfigurationPage, ProgressPage, FinishPage };

	ProgressDialogPage *ppage;
};


namespace InstallApplicationWizardPrivate {

	class choicePage : public QWizardPage
	{
		Q_OBJECT
		public:
			choicePage(QWidget *parent = 0);
	};

	class locateSetupPage : public QWizardPage
	{
		Q_OBJECT
		public:
			locateSetupPage(QWidget *parent = 0);
			bool validatePage();

		public slots:
			void on_browseButton_clicked();
	};
}

#endif
