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
#include <QMessageBox>
#include <QMetaObject>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QModelIndex>
#include <QSettings>
#include "createconfigurationwizard.h"
#include "commonpages.h"
#include "../diroperations.h"
#include "../models/configmodel.h"
#include "../models/configuration.h"
#include "../models/modelindex.h"
#include "../wine/wineprefixcreate.h"
#include "../widgets/modelitemselectcombobox.h"


CreateConfigurationProcess::CreateConfigurationProcess(const QString & name, quint16 installationID,
		QProgressDialog *pd, QObject *parent)
	: QObject(parent)
{
	this->installationID = installationID;
	newConfigurationDir = QDir(cm->contentDir.absoluteFilePath(name));
	this->pd = pd;
	allOK = false;
}

void CreateConfigurationProcess::run()
{
	allOK = wineprefixcreate(newConfigurationDir, installationID, pd);
	if (allOK) {
		if (pd) pd->setLabelText(tr("Registering new configuration"));
		quint16 id = cm->generateID(newConfigurationDir.dirName());
		QSettings s( newConfigurationDir.absolutePath() + "/qtwine.ini", QSettings::IniFormat );
		s.setValue("id", id);
		s.setValue("installation_id", installationID);
		s.sync();
		Configuration *newCfg = new Configuration(newConfigurationDir.absolutePath(), cm);
		cm->insert(newCfg);
	}
	emit finished();
}

//////////////////////////////

CopyConfigurationProcess::CopyConfigurationProcess(const QString & name, quint16 configToCopyID,
		QProgressDialog *pd, QObject *parent)
	: QObject(parent)
{
	newConfigurationDir = QDir(cm->contentDir.absoluteFilePath(name));
	this->pd = pd;
	allOK = false;
	sourceDir.setPath( ModelIndex(configToCopyID, ModelManager::ConfigurationsModel)
				.propertyData("Path").toString() );
}

void CopyConfigurationProcess::run()
{
	allOK = DirOperations::recursiveCpDir(sourceDir, newConfigurationDir, pd);
	if (allOK) {
		if (pd) pd->setLabelText(tr("Registering new configuration"));
		quint16 id = cm->generateID(newConfigurationDir.dirName());
		QSettings s( newConfigurationDir.absolutePath() + "/qtwine.ini", QSettings::IniFormat );
		s.setValue("id", id);
		s.sync();
		Configuration *newCfg = new Configuration(newConfigurationDir.absolutePath(), cm);
		cm->insert(newCfg);
	}
	emit finished();
}

///////////////////////////////

CreateConfigurationWizard::CreateConfigurationWizard(QWidget *parent)
 : QWizard(parent)
{
	setOption(QWizard::DisabledBackButtonOnLastPage);

	LabelPage *startPage = new LabelPage(this);
	startPage->setTitle(tr("Welcome!"));
	startPage->setText(tr("This wizard will guide you through the process"
			" of creating a new wine configuration. Click Next to continue."));
	setPage(StartPage, startPage);

	setPage(ChoicePage, new CreateConfigurationWizardPrivate::choicePage(this));
	setPage(NewConfigurationPage,
		new CreateConfigurationWizardPrivate::newConfigurationPropertiesPage(this));
	setPage(CopyPage, new CreateConfigurationWizardPrivate::copyPropertiesPage(this));

	ppage = new ProgressDialogPage(this);
	ppage->setTitle(tr("Creating the configuration"));
	ppage->setSubTitle(tr("Please wait while the new wine configuration is being created"));
	setPage(ProgressPage, ppage);

	LabelPage *finishPage = new LabelPage(this, "FinishPageText");
	finishPage->setTitle(tr("Finished"));
	setPage(FinishPage, finishPage);
}

void CreateConfigurationWizard::initializePage(int id)
{
	static QObject *process = 0;

	if (id == ProgressPage) {
		//FIXME next and back are not disabled here.... (qt bug?)
		button(QWizard::BackButton)->setEnabled(false);
		button(QWizard::NextButton)->setEnabled(false);
		button(QWizard::CancelButton)->setEnabled(false);

		if ( field("createChoice").toBool() ) {
			process = new CreateConfigurationProcess( field("newConfigName").toString(),
								  field("installation").toUInt(),
								  ppage->progressDialog,
								  this );
		} else {
			process = new CopyConfigurationProcess( field("copyName").toString(),
								field("configuration").toUInt(),
								ppage->progressDialog,
								this );
		}
		connect(process, SIGNAL(finished()), this, SLOT(next()));
		QMetaObject::invokeMethod(process, "run", Qt::QueuedConnection);

	} else if ( id == FinishPage ) {
		button(QWizard::CancelButton)->setEnabled(false);
		bool ok = false;
		QMetaObject::invokeMethod(process, "success", Qt::DirectConnection,
					  Q_RETURN_ARG(bool, ok));
		if ( ok )
			setField("FinishPageText",tr("You have successfully created a new wine configuration"));
		else
			setField("FinishPageText",tr("Creating a new configuration has failed."));
	} else {
		QWizard::initializePage(id);
	}
}

int CreateConfigurationWizard::nextId() const
{
	switch(currentId()) {
		case StartPage:
			return ChoicePage;
		case ChoicePage:
			return field("createChoice").toBool() ? NewConfigurationPage : CopyPage;
		case NewConfigurationPage:
		case CopyPage:
			return ProgressPage;
		case ProgressPage:
			return FinishPage;
		default:
			return -1;
	}
}

//////////////////////////

CreateConfigurationWizardPrivate::choicePage::choicePage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Select what you want to do"));
	setSubTitle(tr("To create a new configuration, there are two methods."
			" Please select the method that you want to use."));

	QButtonGroup *group = new QButtonGroup(this);
	QRadioButton *createRadio = new QRadioButton(tr("Create a new configuration from scratch"), this);
	QRadioButton *copyRadio = new QRadioButton(tr("Copy an existing configuration with all"
							" applications installed in it."), this);

	group->addButton(createRadio);
	group->addButton(copyRadio);
	createRadio->setChecked(true);
	
	QVBoxLayout *vlay = new QVBoxLayout;
	vlay->addWidget(createRadio);
	vlay->addWidget(copyRadio);
	setLayout(vlay);

	registerField("createChoice", createRadio, "checked", SIGNAL(toggled(bool)) );
}

CreateConfigurationWizardPrivate::newConfigurationPropertiesPage::newConfigurationPropertiesPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Enter details for the new configuration"));
	setSubTitle(tr("Here you have to enter the name of the new configuration and"
			" the wine installation that it will use."));

	QLabel *nameLabel = new QLabel(tr("New configuration's name (this is usually the name"
				" of the application that you want to install in there):"), this);
	nameLabel->setWordWrap(true);
	QLineEdit *nameEdit = new QLineEdit(this);
	nameLabel->setBuddy(nameEdit);

	QLabel *installationLabel = new QLabel(tr("Uses Installation:"), this);
	ModelItemSelectComboBox *installationBox =
			new ModelItemSelectComboBox(ModelManager::InstallationsModel,this);
	installationLabel->setBuddy(installationBox);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(nameLabel);
	layout->addWidget(nameEdit);
	layout->addWidget(installationLabel);
	layout->addWidget(installationBox);

	registerField("newConfigName*", nameEdit);
	registerField("installation*", installationBox, "CurrentID", SIGNAL(currentIdChanged(quint16)));
}

bool CreateConfigurationWizardPrivate::newConfigurationPropertiesPage::validatePage()
{
	ModelPointer<ConfigModel> m;
	// check if the name of the new configuration already exists
	if ( m->exists(field("newConfigName").toString()) )
	{
		QMessageBox::critical(this, tr("Configuration exists"),
				      tr("A wine configuration with this name already exists."
				      " The process cannot continue. Please choose a different name.") );
			return false;
	}

	return true;
}

CreateConfigurationWizardPrivate::copyPropertiesPage::copyPropertiesPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Enter details for the new configuration"));
	setSubTitle(tr("Here you have to select the configuration that will be copied"
			" and the name that the resulting configuration (the copy) will have."));

	QLabel *configLabel = new QLabel(tr("Copy this configuration:"), this);
	ModelItemSelectComboBox *configBox =
			new ModelItemSelectComboBox(ModelManager::ConfigurationsModel, this);
	configLabel->setBuddy(configBox);

	QLabel *nameLabel = new QLabel(tr("Name of the copy:"), this);
	QLineEdit *nameEdit = new QLineEdit(this);
	nameLabel->setBuddy(nameEdit);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(configLabel);
	layout->addWidget(configBox);
	layout->addWidget(nameLabel);
	layout->addWidget(nameEdit);

	registerField("copyName*", nameEdit);
	registerField("configuration*", configBox, "CurrentID", SIGNAL(currentIdChanged(quint16)));
}

bool CreateConfigurationWizardPrivate::copyPropertiesPage::validatePage()
{
	ModelPointer<ConfigModel> m;
	// check if the name of the new configuration already exists
	if ( m->exists(field("copyName").toString()) )
	{
		QMessageBox::critical(this, tr("Configuration exists"),
				      tr("A wine configuration with this name already exists."
				      " The process cannot continue. Please choose a different name.") );
		return false;
	}

	return true;
}
