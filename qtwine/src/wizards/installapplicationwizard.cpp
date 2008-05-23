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
#include <QtDebug>
#include "../widgets/modelitemselectcombobox.h"
#include "createconfigurationwizard.h"
#include "commonpages.h"
#include "installapplicationwizard.h"

InstallApplicationWizard::InstallApplicationWizard(QWidget *parent)
	: QWizard(parent)
{
	setOption(QWizard::DisabledBackButtonOnLastPage);

	LabelPage *startPage = new LabelPage(this);
	startPage->setTitle(tr("Welcome!"));
	startPage->setText(tr("This wizard will help you to install a windows application on your system"
				" using wine. Click Next to continue."));
	setPage(StartPage, startPage);

	setPage(LocateSetupPage, new InstallApplicationWizardPrivate::locateSetupPage(this));
	setPage(ChoicePage, new InstallApplicationWizardPrivate::choicePage(this));
	setPage(NewConfigurationPage,
		new CreateConfigurationWizardPrivate::newConfigurationPropertiesPage(this));

	ppage = new ProgressDialogPage(this);
	ppage->setTitle(tr("Creating the configuration"));
	ppage->setSubTitle(tr("Please wait while the new wine configuration is being created"));
	setPage(ProgressPage, ppage);

	LabelPage *finishPage = new LabelPage(this, "FinishPageText");
	finishPage->setTitle(tr("Finished!"));
	setPage(FinishPage, finishPage);
}

void InstallApplicationWizard::setSetupExecutable(const QString & filename)
{
	setField("setupExecutable", filename);
}

int InstallApplicationWizard::nextId() const
{
	switch(currentId()) {
		case StartPage:
			return field("setupExecutable").toString().isEmpty() ? LocateSetupPage : ChoicePage;
		case LocateSetupPage:
			return ChoicePage;
		case ChoicePage:
			return field("CreateNewConfiguration").toBool() ? NewConfigurationPage : FinishPage;
		case NewConfigurationPage:
			return ProgressPage;
		case ProgressPage:
			return FinishPage;
		default:
			return -1;
	}

}

void InstallApplicationWizard::initializePage(int id)
{
	static QObject *process;

	switch(id) {
		case ProgressPage:
			//FIXME next and back are not disabled here.... (qt bug?)
			button(QWizard::BackButton)->setEnabled(false);
			button(QWizard::NextButton)->setEnabled(false);
			button(QWizard::CancelButton)->setEnabled(false);

			process = new CreateConfigurationProcess( field("newConfigName").toString(),
								  field("installation").toUInt(),
								  ppage->progressDialog,
								  this );
			connect(process, SIGNAL(finished()), this, SLOT(next()));
			QMetaObject::invokeMethod(process, "run", Qt::QueuedConnection);
			break;
		case FinishPage:
			button(QWizard::CancelButton)->setEnabled(false);

			if ( hasVisitedPage(ProgressPage) ) {
				bool ok = false;
				QMetaObject::invokeMethod(process, "success", Qt::DirectConnection,
							  Q_RETURN_ARG(bool, ok));
				if ( ok )
					setField("FinishPageText", tr("You have successfully created a"
									" new wine configuration.\n\n"));
				else
					setField("FinishPageText", tr("Creating a new configuration"
									" has failed.\n\n"));
			}

			setField("FinishPageText", field("FinishPageText").toString() +
					tr("The installation of the windows application that you selected"
					" is going to start now. Follow the instructions just like in windows."
					"\n\nIf prompted, select the option to add a new icon on the desktop "
					"so that a shortcut for the application is created automatically on the"
					" \"Installed Applications\" tab of the main window. No icons will be"
					" placed on your real desktop.") );
			//TODO start the installation

			break;
		default:
			QWizard::initializePage(id);
	}
}

InstallApplicationWizardPrivate::locateSetupPage::locateSetupPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Locate the installation executable"));
	setSubTitle(tr("Here you have to enter the file path to the setup.exe or"
			" install.exe file that will install your program."));

	QLabel *setupLabel = new QLabel(tr("File path of the installation executable"
					" (usually setup.exe or install.exe)"), this);
	QLineEdit *setupEdit = new QLineEdit(this);
	setupLabel->setBuddy(setupEdit);

	QToolButton *browseButton = new QToolButton(this);
	browseButton->setIcon(QIcon(":/icons/browse.png"));

	QHBoxLayout *hlay = new QHBoxLayout;
	hlay->addWidget(setupEdit);
	hlay->addWidget(browseButton);

	QVBoxLayout *vlay = new QVBoxLayout(this);
	vlay->addWidget(setupLabel);
	vlay->addLayout(hlay);

	registerField("setupExecutable*", setupEdit);
	connect(browseButton, SIGNAL(clicked()), SLOT(on_browseButton_clicked()) );
}

void InstallApplicationWizardPrivate::locateSetupPage::on_browseButton_clicked()
{
	QString file = QFileDialog::getOpenFileName( this, tr("Select the installation executable"),
						QDir::homePath(), tr("WINE executables (*.exe *.exe.so)"));
	if (!file.isEmpty())
		setField("setupExecutable", file);
}

bool InstallApplicationWizardPrivate::locateSetupPage::validatePage()
{
	if ( !QFile::exists(field("setupExecutable").toString()) ) {
		qDebug() << "The installation executable that you specified does not exist.";
		return false;
	}

	return true;
}

InstallApplicationWizardPrivate::choicePage::choicePage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Select configuration"));
	setSubTitle(tr("Select the wine configuration that wine will use for the installation process."
			" The new application will be installed in this configuration."));

	QButtonGroup *group = new QButtonGroup(this);
	//### Radio buttons do not have word wrap so they need a \n every about 70 characters.
	QRadioButton *createRadio = new QRadioButton(tr("Create a new configuration"
			" (recommended if you\nwant to install a new windows application)"), this);
	QRadioButton *useRadio = new QRadioButton(tr("Use an existing configuration (use this if you want"
			" to install a\nsoftware patch, update, or an application that must cooperate with"
			"\nan already installed application, for example a plugin)"), this);

	group->addButton(createRadio);
	group->addButton(useRadio);
	createRadio->setChecked(true);

	QFrame *frame = new QFrame(this);
	QLabel *label = new QLabel(tr("Existing configuration to use:"), frame); //TODO validatePage() to check this!!
	ModelItemSelectComboBox *combo = new ModelItemSelectComboBox(ModelManager::ConfigurationsModel, frame);
	frame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	QHBoxLayout *hlay = new QHBoxLayout(frame);
	hlay->addWidget(label);
	hlay->addWidget(combo);

	QVBoxLayout *vlay = new QVBoxLayout(this);
	vlay->addWidget(createRadio);
	vlay->addWidget(useRadio);
	vlay->addWidget(frame);

	frame->setEnabled(false);
	connect(useRadio, SIGNAL(toggled(bool)), frame, SLOT(setEnabled(bool)) );
	registerField("CreateNewConfiguration", createRadio, "checked", SIGNAL(toggled(bool)) );
	registerField("ConfigurationToUse", combo, "CurrentID", SIGNAL(currentIdChanged(quint16)) );
}
