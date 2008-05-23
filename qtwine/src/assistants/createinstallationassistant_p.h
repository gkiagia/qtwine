/***************************************************************************
 *   Copyright (C) 2007-2008 by George Kiagiadakis                         *
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
#ifndef CREATEINSTALLATIONASSISTANT_P_H
#define CREATEINSTALLATIONASSISTANT_P_H

#include <QWizardPage>

namespace CreateInstallationAssistantPrivate {

class choicePage : public QWizardPage
{
public:
	choicePage(QWidget *parent = 0);
};

class basicInformationPage : public QWizardPage
{
public:
	basicInformationPage(QWidget *parent = 0);
	bool validatePage();
};

class advancedInformationPage : public QWizardPage
{
public:
	advancedInformationPage(QWidget *parent = 0);
	void initializePage();
	bool validatePage();
};

}

#endif
