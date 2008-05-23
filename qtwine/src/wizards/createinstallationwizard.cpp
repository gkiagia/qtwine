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
#include <QLineEdit>
#include <QToolButton>
#include <QLabel>
#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include "../models/installationmodel.h"
#include "../models/installation.h"
#include "../models/modelpointer.h"
#include "createinstallationwizard.h"
#include "commonpages.h"

CreateInstallationWizard::CreateInstallationWizard(QWidget *parent)
	: QWizard(parent)
{
	setOption(QWizard::DisabledBackButtonOnLastPage);

	LabelPage *startPage = new LabelPage(this);
	startPage->setTitle(tr("Welcome!"));
	startPage->setText(tr("This wizard will help you to create or import a new wine installation"
				" into QtWine. Click Next to continue."));
	setPage(StartPage, startPage);

	setPage(ChoicePage, new CreateInstallationWizardPrivate::choicePage(this));
	setPage(BasicInformationPage, new CreateInstallationWizardPrivate::basicInformationPage(this));
	setPage(AdvancedInformationPage, new CreateInstallationWizardPrivate::advancedInformationPage(this));

	LabelPage *finishPage = new LabelPage(this, "FinishLabelText");
	finishPage->setTitle(tr("Finished!"));
	finishPage->setText(tr("You have successfully created a new wine installation."
			" Now set a configuration to use this installation to see it in action."));
	setPage(FinishPage, finishPage);
}

int CreateInstallationWizard::nextId() const
{
	switch(currentId()) {
		case StartPage:
			return ChoicePage;
		case ChoicePage:
			return field("importChoice").toBool() ? BasicInformationPage : -1;
		case BasicInformationPage:
			return AdvancedInformationPage;
		case AdvancedInformationPage:
			return FinishPage;
		default:
			return -1;
	}
}

void CreateInstallationWizard::initializePage(int id)
{
	if ( id == FinishPage ) {
		ModelPointer<InstallationModel> im;
		QModelIndex newItem = im->createInstallation( field("installationName").toString(),
							      field("installationPrefix").toString() );
		if ( newItem.isValid() ) {
			im->setPropertyData(newItem, "WineLoader", field("wineloader").toString() );
			im->setPropertyData(newItem, "WineServer", field("wineserver").toString() );
			im->setPropertyData(newItem, "WineDllPath", field("winedllpath").toString() );

			setField("FinishLabelText", tr("You have successfully created a new wine installation."
				" Now set a configuration to use this installation to see it in action."));
		} else {
			setField("FinishLabelText", tr("Creating a new installation has failed"
							" for unknown reason"));
		}
	} else {
		QWizard::initializePage(id);
	}
}


CreateInstallationWizardPrivate::choicePage::choicePage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Select method of creation"));
	setSubTitle(tr("You can either use an existing installation of wine that you installed"
			" manually or from a distribution package, or compile wine from its sources."));

	QButtonGroup *group = new QButtonGroup(this);
	QRadioButton *importRadio = new QRadioButton(tr("Use an existing installation"), this);
	QRadioButton *compileRadio = new QRadioButton(tr("Compile wine from its source code"
							" (not implmented yet)"), this);

	group->addButton(importRadio);
	group->addButton(compileRadio);
	importRadio->setChecked(true);
	compileRadio->setEnabled(false); //TODO compiling not implemented yet
	
	QVBoxLayout *vlay = new QVBoxLayout;
	vlay->addWidget(importRadio);
	vlay->addWidget(compileRadio);
	setLayout(vlay);

	registerField("importChoice", importRadio, "checked", SIGNAL(toggled(bool)) );
}

CreateInstallationWizardPrivate::basicInformationPage::basicInformationPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Input basic information"));
	setSubTitle(tr("Here you need to enter some basic information about the new installation."));

	QLabel *nameLabel = new QLabel(tr("Name:"), this);
	QLineEdit *nameEdit = new QLineEdit(this);
	QLabel *prefixLabel = new QLabel(tr("Prefix directory path:"), this);
	QLineEdit *prefixEdit = new QLineEdit(this);
	QToolButton *browseButton = new QToolButton(this);

	QHBoxLayout *hlay = new QHBoxLayout;
	hlay->addWidget(prefixEdit);
	hlay->addWidget(browseButton);

	QVBoxLayout *vlay = new QVBoxLayout;
	vlay->addWidget(nameLabel);
	vlay->addWidget(nameEdit);
	vlay->addWidget(prefixLabel);
	vlay->addLayout(hlay);
	setLayout(vlay);

	registerField("installationName", nameEdit);
	registerField("installationPrefix", prefixEdit);

	connect(browseButton, SIGNAL(clicked()), this, SLOT(on_browseButton_clicked()) );
}

bool CreateInstallationWizardPrivate::basicInformationPage::validatePage()
{
	if ( ! QFile::exists(field("installationPrefix").toString()) ) {
		QMessageBox::critical(this, tr("Directory does not exist"), tr("The prefix directory"
				" that you have specified does not exist. Please specify a valid directory") );
		return false;
	}

	ModelPointer<InstallationModel> im;
	if ( im->exists(field("installationName").toString()) ) {
		QMessageBox::critical(this, tr("Installation exists"),
					tr("A wine installation with this name already exists."
					" The process cannot continue. Please choose a different name.") );
		return false;
	}

	return true;
}

void CreateInstallationWizardPrivate::basicInformationPage::on_browseButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory( this, tr("Select a directory"), "/",
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
	if (!dir.isEmpty())
		setField("installationPrefix", dir);
}

CreateInstallationWizardPrivate::advancedInformationPage::advancedInformationPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Input advanced information"));
	setSubTitle(tr("Here you can optionally input some extra information about the new installation."));

	QLabel *loaderLabel = new QLabel(tr("Path to the wine executable ($WINELOADER):"), this);
	QLabel *serverLabel = new QLabel(tr("Path to the wineserver executable ($WINESERVER):"), this);
	QLabel *dllpathLabel = new QLabel(tr("Path to the directory where wine stores its dlls"
										" ($WINEDLLPATH):"), this);
	QLineEdit *loaderEdit = new QLineEdit(this);
	QLineEdit *serverEdit = new QLineEdit(this);
	QLineEdit *dllpathEdit = new QLineEdit(this);

	QToolButton *loaderBrowseButton = new QToolButton(this);
	QToolButton *serverBrowseButton = new QToolButton(this);
	QToolButton *dllpathBrowseButton = new QToolButton(this);

	QHBoxLayout *hlay1 = new QHBoxLayout;
	hlay1->addWidget(loaderEdit);
	hlay1->addWidget(loaderBrowseButton);
	
	QHBoxLayout *hlay2 = new QHBoxLayout;
	hlay2->addWidget(serverEdit);
	hlay2->addWidget(serverBrowseButton);
	
	QHBoxLayout *hlay3 = new QHBoxLayout;
	hlay3->addWidget(dllpathEdit);
	hlay3->addWidget(dllpathBrowseButton);

	QVBoxLayout *vlay = new QVBoxLayout;
	vlay->addWidget(loaderLabel);
	vlay->addLayout(hlay1);
	vlay->addWidget(serverLabel);
	vlay->addLayout(hlay2);
	vlay->addWidget(dllpathLabel);
	vlay->addLayout(hlay3);
	setLayout(vlay);

	registerField("wineloader", loaderEdit);
	registerField("wineserver", serverEdit);
	registerField("winedllpath", dllpathEdit);

	connect(loaderBrowseButton, SIGNAL(clicked()), this, SLOT(on_loaderBrowseButton_clicked()) );
	connect(serverBrowseButton, SIGNAL(clicked()), this, SLOT(on_serverBrowseButton_clicked()) );
	connect(dllpathBrowseButton, SIGNAL(clicked()), this, SLOT(on_dllpathBrowseButton_clicked()) );
}

void CreateInstallationWizardPrivate::advancedInformationPage::initializePage()
{
	setField("wineloader", Installation::defaultWineLoader(field("installationPrefix").toString()) );
	setField("wineserver", Installation::defaultWineServer(field("installationPrefix").toString()) );
	setField("winedllpath", Installation::defaultWineDllPath(field("installationPrefix").toString()) );
}

bool CreateInstallationWizardPrivate::advancedInformationPage::validatePage()
{
	if ( !QFile::exists(field("wineloader").toString()) or
	     !QFileInfo(field("wineloader").toString()).isFile() ) {
		QMessageBox::critical(this, tr("Wineloader does not exist"), tr("The file %1 does not exist."
					"Please specify the correct path to the wine binary.")
					.arg(field("wineloader").toString()));
		return false;
	}

	if ( !QFile::exists(field("wineserver").toString()) or
	     !QFileInfo(field("wineserver").toString()).isFile() ) {
		QMessageBox::critical(this, tr("Wineserver does not exist"), tr("The file %1 does not exist."
					"Please specify the correct path to the wineserver binary.")
					.arg(field("wineserver").toString()));
		return false;
	}

	if ( !QFile::exists(field("winedllpath").toString()) or
	     !QFileInfo(field("winedllpath").toString()).isDir() ) {
		QMessageBox::critical(this, tr("WineDllPath does not exist"), tr("The directory %1 does not"
					" exist. Please specify the correct path to the wine dll"
					" directory.").arg(field("winedllpath").toString()));
		return false;
	}
	return true;
}

void CreateInstallationWizardPrivate::advancedInformationPage::on_loaderBrowseButton_clicked()
{
	QString file = QFileDialog::getOpenFileName( this, tr("Select the wine executable"),
							field("installationPrefix").toString(), QString(),
							NULL, QFileDialog::DontResolveSymlinks );
	if (!file.isEmpty())
		setField("wineloader", file);
}

void CreateInstallationWizardPrivate::advancedInformationPage::on_serverBrowseButton_clicked()
{
	QString file = QFileDialog::getOpenFileName( this, tr("Select the wineserver executable"),
							field("installationPrefix").toString(), QString(),
							NULL, QFileDialog::DontResolveSymlinks );
	if (!file.isEmpty())
		setField("wineserver", file);
}

void CreateInstallationWizardPrivate::advancedInformationPage::on_dllpathBrowseButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory( this, tr("Select a directory"),
			 			field("installationPrefix").toString(),
						QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
	if (!dir.isEmpty())
		setField("winedllpath", dir);
}
