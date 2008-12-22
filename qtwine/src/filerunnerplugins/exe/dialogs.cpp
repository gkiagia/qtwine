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
#include "dialogs.h"
#include "../../qtwineapplication.h"
#include "../../widgets/winedlloverridesrequester.h"

#include "argumentslist.h"

#include <QFormLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QRegExp>
#include <QLabel>
#include <QCommandLinkButton>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QFileInfo>

#include <KMessageBox>
#include <KUrlRequester>
#include <KLocalizedString>
#include <KLineEdit>
#include <KSqueezedTextLabel>

RunProgramDialog::RunProgramDialog(QWidget *parent)
    : KDialog(parent)
{
    setButtons(KDialog::Ok | KDialog::Cancel);
    setCaption(makeStandardCaption(i18n("Run program in wine"), this));

    QWidget *mainWidget = new QWidget(this);
    setMainWidget(mainWidget);

    QVBoxLayout *mainVlay = new QVBoxLayout(mainWidget);
    QFormLayout *formLayout = new QFormLayout;
    mainVlay->addLayout(formLayout);

    m_executableLabel = new KSqueezedTextLabel(mainWidget);
    formLayout->addRow(i18n("&Executable:"), m_executableLabel);

    m_argumentsEdit = new KLineEdit(mainWidget);
    formLayout->addRow(i18n("&Arguments:"), m_argumentsEdit);

    m_workdirRequester = new KUrlRequester(mainWidget);
    m_workdirRequester->setMode(KFile::Directory | KFile::LocalOnly | KFile::ExistingOnly);
    formLayout->addRow(i18n("&Working directory:"), m_workdirRequester);

    m_configComboBox = new QComboBox(mainWidget);
    m_configComboBox->setModel(qtwineApp->wineConfigurationsModel());
    m_configComboBox->setModelColumn(qtwineApp->wineConfigurationsModel()->fieldIndex("name"));
    m_configComboBox->setCurrentIndex(0);
    formLayout->addRow(i18n("Use wine confi&guration:"), m_configComboBox);

    m_dllOverridesRequester = new WineDllOverridesRequester(mainWidget);
    formLayout->addRow(i18n("Wine &dll overrides:"), m_dllOverridesRequester);

    m_logfileRequester = new KUrlRequester(mainWidget);
    m_logfileRequester->setMode(KFile::File | KFile::LocalOnly);
    formLayout->addRow(i18n("&Log output to a file:"), m_logfileRequester);

    // options groupbox
    QGroupBox *optionsGroup = new QGroupBox(i18n("Options"), mainWidget);
    QVBoxLayout *optionsVlay = new QVBoxLayout(optionsGroup);

    m_terminalBox = new QCheckBox(i18n("Show output in a terminal"), optionsGroup);
    optionsVlay->addWidget(m_terminalBox);

    m_wineconsoleBox = new QCheckBox(i18n("Use wineconsole (for CUI/DOS programs)"), optionsGroup);
    optionsVlay->addWidget(m_wineconsoleBox);

    resizeLayout(optionsVlay, marginHint(), spacingHint());
    mainVlay->addWidget(optionsGroup);
    resizeLayout(mainVlay, marginHint(), spacingHint());
}

QString RunProgramDialog::executable() const
{
    return m_executableLabel->text();
}

void RunProgramDialog::setExecutable(const QString & url)
{
    Q_ASSERT(!url.isEmpty()); //executable is the only field that MUST NOT be empty.
    m_executableLabel->setText(url);
    setWorkingDirectory(QFileInfo(url).absolutePath());
}

QtWine::ArgumentsList RunProgramDialog::arguments() const
{
    return QtWine::ArgumentsList::fromSingleString(m_argumentsEdit->text());
}

void RunProgramDialog::setArguments(const QtWine::ArgumentsList & args)
{
    m_argumentsEdit->setText(args.toSingleString());
}

QString RunProgramDialog::workingDirectory() const
{
    return m_workdirRequester->url().path();
}

void RunProgramDialog::setWorkingDirectory(const QString & dir)
{
    if ( dir.isEmpty() ) return; //keep the url set by setExecutable()
    m_workdirRequester->setUrl(dir);
}

int RunProgramDialog::wineConfigurationId() const
{
    return qtwineApp->wineConfigurationsModel()->rowToId(m_configComboBox->currentIndex());
}

void RunProgramDialog::setWineConfigurationId(int id)
{
    m_configComboBox->setCurrentIndex(qtwineApp->wineConfigurationsModel()->idToRow(id));
}

QtWine::WineDllOverrides RunProgramDialog::wineDllOverrides() const
{
    return m_dllOverridesRequester->dllOverrides();
}

void RunProgramDialog::setWineDllOverrides(const QtWine::WineDllOverrides overrides)
{
    m_dllOverridesRequester->setDllOverrides(overrides);
}

KUrl RunProgramDialog::logFile() const
{
    return m_logfileRequester->url();
}

void RunProgramDialog::setLogFile(const KUrl & logfile)
{
    m_logfileRequester->setUrl(logfile);
}

bool RunProgramDialog::runInTerminal() const
{
    return m_terminalBox->isChecked();
}

void RunProgramDialog::setRunInTerminal(bool enabled)
{
    m_terminalBox->setChecked(enabled);
}

bool RunProgramDialog::isConsoleApplication() const
{
    return m_wineconsoleBox->isChecked();
}

void RunProgramDialog::setIsConsoleApplication(bool enabled)
{
    m_wineconsoleBox->setChecked(enabled);
}



ExeRunnerActionsDialog::ExeRunnerActionsDialog(QWidget *parent)
    : KDialog(parent)
{
    setButtons(KDialog::Cancel);
    setCaption(i18n("Select action"));

    QWidget *mainWidget = new QWidget(this);
    setMainWidget(mainWidget);

    QLabel *info = new QLabel(mainWidget);
    info->setWordWrap(true);
    info->setText(i18n("Please select the action that you would like to do with this executable."));

    QCommandLinkButton *runButton = new QCommandLinkButton(mainWidget);
    runButton->setText(i18n("Run this program"));
    runButton->setDescription(i18n("Select this option if you want just to run this program "
                            "without installing it, or if it is already installed in a certain "
                            "virtual windows installation."));

    QCommandLinkButton *installButton = new QCommandLinkButton(mainWidget);
    installButton->setText(i18n("Install this program"));
    installButton->setDescription(i18n("Select this option if this program is an installer."));

    QCommandLinkButton *createShortcutButton = new QCommandLinkButton(mainWidget);
    createShortcutButton->setText(i18n("Add this program in QtWine's shortcuts"));

    QVBoxLayout *layout = new QVBoxLayout(mainWidget);
    layout->addWidget(info);
    layout->addWidget(runButton);
    layout->addWidget(installButton);
    layout->addWidget(createShortcutButton);

    QSignalMapper *mapper = new QSignalMapper(this);
    mapper->setMapping(runButton, (int)ExeRunnerPluginWithUi::Run);
    mapper->setMapping(installButton, (int)ExeRunnerPluginWithUi::Install);
    mapper->setMapping(createShortcutButton, (int)ExeRunnerPluginWithUi::CreateShortcut);

    connect(runButton, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(installButton, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(createShortcutButton, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(slotMapped(int)));
}

void ExeRunnerActionsDialog::slotMapped(int who)
{
    accept();
    emit actionSelected( (ExeRunnerPluginWithUi::Action) who );
}

void ExeRunnerActionsDialog::reject()
{
    KDialog::reject();
    emit actionSelected( ExeRunnerPluginWithUi::NoAction );
}

#include "dialogs.moc"
