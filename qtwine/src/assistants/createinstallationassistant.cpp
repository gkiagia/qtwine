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
#include "createinstallationassistant.h"
#include "createinstallationassistant_p.h"
#include "commonpages.h"
#include "../qtwineapplication.h"

#include "wineinstallation.h"

#include <QLabel>
#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QFile>

#include <KMessageBox>
#include <KUrlRequester>
#include <KLocalizedString>
#include <KLineEdit>
#include <KDialog>


CreateInstallationAssistant::CreateInstallationAssistant(QWidget *parent)
	: QWizard(parent)
{
	setOption(QWizard::DisabledBackButtonOnLastPage);
	setWindowTitle(KDialog::makeStandardCaption(i18n("Create wine installation assistant"), this));

	LabelPage *startPage = new LabelPage(this);
	startPage->setTitle(i18n("Welcome!"));
	startPage->setText(i18n("This assistant will help you to create or import a new wine installation"
				" into QtWine. Click Next to continue."));
	setPage(StartPage, startPage);

	using namespace CreateInstallationAssistantPrivate;
	setPage(ChoicePage, new choicePage(this));
	setPage(BasicInformationPage, new basicInformationPage(this));
	setPage(AdvancedInformationPage, new advancedInformationPage(this));

	LabelPage *finishPage = new LabelPage(this, "FinishLabelText");
	finishPage->setTitle(i18nc("the assistant has finished", "Finished!"));
	finishPage->setText(i18n("You have successfully created a new wine installation."
			" Now set a configuration to use this installation to see it in action."));
	setPage(FinishPage, finishPage);
}

int CreateInstallationAssistant::nextId() const
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

void CreateInstallationAssistant::initializePage(int id)
{
	if ( id == FinishPage ) {
		using namespace QtWine;
		WineInstallation newInstallation(field("installationPrefix").value<KUrl>().path());
		newInstallation.setWineLoader(field("wineloader").value<KUrl>().path());
		newInstallation.setWineServer(field("wineserver").value<KUrl>().path());
		newInstallation.setWineDllPath(field("winedllpath").value<KUrl>().path());

		WineInstallationsProvider *p = installationsProvider;
		if ( !p->importInstallation( field("installationName").toString(), newInstallation ) )
			setField("FinishLabelText", i18n("Creating a new installation has failed"
							" for unknown reason"));
	} else {
		QWizard::initializePage(id);
	}
}


CreateInstallationAssistantPrivate::choicePage::choicePage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(i18n("Select method of creation"));
	setSubTitle(i18n("You can either use an existing installation of wine that you installed"
			" manually or from a distribution package, or compile wine from its sources."));

	QButtonGroup *group = new QButtonGroup(this);
	QRadioButton *importRadio = new QRadioButton(i18n("Use an existing installation"), this);
	QRadioButton *compileRadio = new QRadioButton(i18n("Compile wine from its source code"
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

CreateInstallationAssistantPrivate::basicInformationPage::basicInformationPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(i18n("Input basic information"));
	setSubTitle(i18n("Here you need to enter some basic information about the new installation."));

	QLabel *nameLabel = new QLabel(i18n("Name:"), this);
	KLineEdit *nameEdit = new KLineEdit(this);
	QLabel *prefixLabel = new QLabel(i18n("Prefix directory path:"), this);
	KUrlRequester *prefixRequester = new KUrlRequester(this);
	prefixRequester->setMode(KFile::Directory | KFile::LocalOnly | KFile::ExistingOnly);

	QVBoxLayout *vlay = new QVBoxLayout;
	vlay->addWidget(nameLabel);
	vlay->addWidget(nameEdit);
	vlay->addWidget(prefixLabel);
	vlay->addWidget(prefixRequester);
	setLayout(vlay);

	registerField("installationName*", nameEdit);

	// FIXME the mandatory property doesn't seem to work at all :S
	registerField("installationPrefix*", prefixRequester, "url", SIGNAL(textChanged(const QString&)));
}

bool CreateInstallationAssistantPrivate::basicInformationPage::validatePage()
{
	if ( !QFile::exists(field("installationPrefix").value<KUrl>().path()) ) {
		KMessageBox::sorry(this, i18n("The prefix directory that you have specified"
				" does not exist. Please specify a valid directory") );
		return false;
	}

	return true;
}

CreateInstallationAssistantPrivate::advancedInformationPage::advancedInformationPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(i18n("Input advanced information"));
	setSubTitle(i18n("Here you can optionally input some extra"
			" information about the new installation."));

	QLabel *loaderLabel = new QLabel(tr("Path to the wine executable ($WINELOADER):"), this);
	QLabel *serverLabel = new QLabel(tr("Path to the wineserver executable ($WINESERVER):"), this);
	QLabel *dllpathLabel = new QLabel(tr("Path to the directory where wine stores its dlls"
									" ($WINEDLLPATH):"), this);
	KUrlRequester *loaderRequester = new KUrlRequester(this);
	loaderRequester->setMode(KFile::File | KFile::LocalOnly | KFile::ExistingOnly);

	KUrlRequester *serverRequester = new KUrlRequester(this);
	serverRequester->setMode(KFile::File | KFile::LocalOnly | KFile::ExistingOnly);

	KUrlRequester *dllpathRequester = new KUrlRequester(this);
	dllpathRequester->setMode(KFile::Directory | KFile::LocalOnly | KFile::ExistingOnly);

	QVBoxLayout *vlay = new QVBoxLayout;
	vlay->addWidget(loaderLabel);
	vlay->addWidget(loaderRequester);
	vlay->addWidget(serverLabel);
	vlay->addWidget(serverRequester);
	vlay->addWidget(dllpathLabel);
	vlay->addWidget(dllpathRequester);
	setLayout(vlay);

	registerField("wineloader", loaderRequester, "url");
	registerField("wineserver", serverRequester, "url");
	registerField("winedllpath", dllpathRequester, "url");
}

void CreateInstallationAssistantPrivate::advancedInformationPage::initializePage()
{
	QtWine::WineInstallation i(field("installationPrefix").value<KUrl>().path());
	setField("wineloader", KUrl(i.wineLoader()) );
	setField("wineserver", KUrl(i.wineServer()) );
	setField("winedllpath", KUrl(i.wineDllPath()) );
}

bool CreateInstallationAssistantPrivate::advancedInformationPage::validatePage()
{
	QString wineLoader = field("wineloader").value<KUrl>().path();
	QString wineServer = field("wineserver").value<KUrl>().path();
	QString wineDllPath = field("winedllpath").value<KUrl>().path();

	if ( !QFile::exists(wineLoader) or !QFileInfo(wineLoader).isFile() ) {
		KMessageBox::sorry(this, i18n("The wine executable that you specified (\"%1\")"
		" does not exist. Please specify the correct path to it.").arg(wineLoader));
		return false;
	}

	if ( !QFile::exists(wineServer) or !QFileInfo(wineServer).isFile() ) {
		KMessageBox::sorry(this, i18n("The wine server executable that you specified (\"%1\")"
		" does not exist. Please specify the correct path to it.").arg(wineServer));
		return false;
	}

	if ( !QFile::exists(wineDllPath) or !QFileInfo(wineDllPath).isDir() ) {
		KMessageBox::sorry(this, i18n("The wine dll directory that you specified (\"%1\")"
		" does not exist. Please specify the correct path to it.").arg(wineServer));
		return false;
	}
	return true;
}
