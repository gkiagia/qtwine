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
#include "runprogramdialog.h"
#include "../qtwineapplication.h"
#include "../widgets/executablerequester.h"
#include "../widgets/winedlloverridesrequester.h"

#include "wineprocess.h"

#include <QFormLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QRegExp>

#include <KMessageBox>
#include <KUrlRequester>
#include <KIcon>
#include <KLocalizedString>
#include <KLineEdit>


RunProgramDialog::RunProgramDialog(QWidget *parent)
    : KPageDialog(parent)
{
    setButtons(KDialog::Ok | KDialog::Cancel);
    setCaption(makeStandardCaption(i18n("Run command in wine"), this));

    QWidget *page1 = new QWidget(this);
    KPageWidgetItem *firstPageItem = addPage(page1, i18n("Run command"));
    firstPageItem->setIcon(KIcon("system-run"));

    QVBoxLayout *mainVlay = new QVBoxLayout(page1);
    QFormLayout *formLayout = new QFormLayout;
    mainVlay->addLayout(formLayout);

    m_executableRequester = new ExecutableRequester(page1);
    connect(m_executableRequester, SIGNAL(urlSelected(KUrl)), this, SLOT(slotExecutableChanged(KUrl)) );
    formLayout->addRow(i18n("&Executable:"), m_executableRequester);

    m_argumentsEdit = new KLineEdit(page1);
    formLayout->addRow(i18n("&Arguments:"), m_argumentsEdit);

    m_workdirRequester = new KUrlRequester(page1);
    m_workdirRequester->setMode(KFile::Directory | KFile::LocalOnly | KFile::ExistingOnly);
    formLayout->addRow(i18n("&Working directory:"), m_workdirRequester);

    m_configComboBox = new QComboBox(page1);
    m_configComboBox->setModel(qtwineApp->wineConfigurationsModel());
    m_configComboBox->setModelColumn(qtwineApp->wineConfigurationsModel()->fieldIndex("name"));
    m_configComboBox->setCurrentIndex(0);
    formLayout->addRow(i18n("Use wine confi&guration:"), m_configComboBox);

    //this is to always start the KFileDialog of the executable requester
    //in the "Program files" directory of the selected configuration
    m_executableRequester->setWineConfiguration(m_configComboBox->currentIndex());
    connect(m_configComboBox, SIGNAL(currentIndexChanged(int)), m_executableRequester, SLOT(setWineConfiguration(int)) );

    m_dllOverridesRequester = new WineDllOverridesRequester(page1);
    formLayout->addRow(i18n("Wine &dll overrides:"), m_dllOverridesRequester);

    m_logfileRequester = new KUrlRequester(page1);
    m_logfileRequester->setMode(KFile::File | KFile::LocalOnly);
    formLayout->addRow(i18n("&Log output to a file:"), m_logfileRequester);

    // options groupbox
    QGroupBox *optionsGroup = new QGroupBox(i18n("Options"), page1);
    QVBoxLayout *optionsVlay = new QVBoxLayout(optionsGroup);

    m_terminalBox = new QCheckBox(i18n("Show output in a terminal"), optionsGroup);
    optionsVlay->addWidget(m_terminalBox);

    m_wineconsoleBox = new QCheckBox(i18n("Use wineconsole (for CUI/DOS programs)"), optionsGroup);
    optionsVlay->addWidget(m_wineconsoleBox);

    m_winedbgBox = new QCheckBox(i18n("Start command in debugger (winedbg)"), optionsGroup);
    optionsVlay->addWidget(m_winedbgBox);

    resizeLayout(optionsVlay, marginHint(), spacingHint());
    mainVlay->addWidget(optionsGroup);
    resizeLayout(mainVlay, marginHint(), spacingHint());
}
#if 0
void RunProgramDialog::setCommand(const QString & command)
{
    m_commandRequester->lineEdit()->setText( command );
}

void RunProgramDialog::setCommand(const QStringList & command)
{
    if ( !command.isEmpty() )
    {
#if 0
        for (int i=0; i < args.size(); i++) {
            args[i].prepend('\"');
            args[i].append('\"');
        }
#endif
        m_commandRequester->lineEdit()->setText(
                command.join("\" \"").prepend('\"').append('\"') );
    }
}
#endif
void RunProgramDialog::accept()
{
    //check for valid values
    if ( m_configComboBox->currentIndex() == -1 ) {
        KMessageBox::sorry(this, i18n("No wine configuration selected. Please select one."));
        return;
    }

    if ( !m_executableRequester->url().isValid() ) {
        KMessageBox::sorry(this, i18n("No valid executable url was specified. Please specify an executable."));
        return;
    }

    //everything is valid, continue to store the session
    //storeSession();

    // split by spaces but exclude spaces in quotes " "
    QStringList arguments = m_argumentsEdit->text().split(QRegExp("\\s(?!\"\\w+\\s\\w+\")"), QString::SkipEmptyParts);

    //start wine
    using namespace QtWine;
    WineConfiguration wcfg =
            qtwineApp->wineConfigurationsModel()->configurationByModelRow(m_configComboBox->currentIndex());

    WineApplication app(m_executableRequester->url().path(), wcfg); //TODO support remote executable
    app << arguments;

    WineDllOverrides dllOverrides = m_dllOverridesRequester->dllOverrides();
    if ( !dllOverrides.isEmpty() )
        app.setWineDllOverrides( dllOverrides );

    app.setWorkingDirectory( m_workdirRequester->url().path() );
    app.setIsConsoleApplication( m_wineconsoleBox->isChecked() );
    app.enableRunInDebugger( m_winedbgBox->isChecked() );

    WineProcess *wine = new WineProcess(app);

    if ( !m_logfileRequester->url().path().isEmpty() )
        wine->setLogFile( m_logfileRequester->url().path() ); //TODO support remote logfiles

    if ( m_terminalBox->isChecked() )
        wine->openTerminal();

    wine->setAutoDeleteEnabled(true);
    wine->startMonitored();

    // exit the dialog
    QDialog::accept();
}

void RunProgramDialog::slotExecutableChanged(const KUrl & newUrl)
{
    m_workdirRequester->setPath(newUrl.directory());
}

#if 0
void RunProgramDialog::loadSession()
{
    QSettings s( globalSettings->getValue("RunProgramDialog/session_file").toString(), QSettings::IniFormat);
    s.beginGroup( executableInfo.fileName() );

    l.configComboBox->setCurrentId( s.value("configurationID", 0).toUInt() );

    l.argsBox->setChecked( s.value("arguments_checkbox_checked", false).toBool() );
    l.argsEdit->setText( s.value("arguments_lineEdit_text").toString() );

    l.workdirBox->setChecked( s.value("workdir_checkbox_checked", false).toBool() );
    if ( s.value("workdir_checkbox_checked", false).toBool() )
        l.workdirEdit->setText( s.value("workdir_lineEdit_text").toString() );

    l.terminalBox->setChecked( s.value("terminal_checkbox_checked", false).toBool() );
    l.wineconsoleBox->setChecked( s.value("wineconsole_checkbox_checked", false).toBool() );

    if ( s.value("advanced_button_checked").toBool() ) {
        l.advancedButton->setChecked(true);
        on_advancedButton_toggled(true);
    }

    //advanced
    l.winedbgBox->setChecked( s.value("winedbg_checkbox_checked", false).toBool() );

    l.installationBox->setChecked( s.value("installation_checkbox_checked", false).toBool() );

    if ( s.value("installation_checkbox_checked", false).toBool() )
        l.installationComboBox->setCurrentId( s.value("installationID", 0).toUInt() );

    l.logBox->setChecked( s.value("log_checkbox_checked", false).toBool() );
    l.logEdit->setText( s.value("log_lineEdit_text").toString() );

    l.dlloverridesBox->setChecked( s.value("dlloverrides_checkbox_checked", false).toBool() );
    l.dlloverridesEdit->setText( s.value("dlloverrides_lineEdit_text").toString() );

    l.debugBox->setChecked( s.value("debug_checkbox_checked", false).toBool() );
    l.debugEdit->setText( s.value("debug_lineEdit_text").toString() );

    s.endGroup();
}

void RunProgramDialog::storeSession()
{
    QSettings s( globalSettings->getValue("RunProgramDialog/session_file").toString(), QSettings::IniFormat);
    s.beginGroup( executableInfo.fileName() );

    s.setValue("configurationID", l.configComboBox->currentId());

    s.setValue("arguments_checkbox_checked", l.argsBox->isChecked());
    s.setValue("arguments_lineEdit_text", l.argsEdit->text());

    s.setValue("workdir_checkbox_checked", l.workdirBox->isChecked());
    if ( l.workdirBox->isChecked() )
        s.setValue("workdir_lineEdit_text", l.workdirEdit->text() );

    s.setValue("terminal_checkbox_checked", l.terminalBox->isChecked());
    s.setValue("wineconsole_checkbox_checked", l.wineconsoleBox->isChecked());

    s.setValue("advanced_button_checked", l.advancedButton->isChecked());

    //advanced
    s.setValue("winedbg_checkbox_checked", l.winedbgBox->isChecked());

    s.setValue("installation_checkbox_checked", l.installationBox->isChecked());

    if ( l.installationBox->isChecked() )
        s.setValue("installationID", l.installationComboBox->currentId());

    s.setValue("log_checkbox_checked", l.logBox->isChecked());
    s.setValue("log_lineEdit_text", l.logEdit->text());

    s.setValue("dlloverrides_checkbox_checked", l.dlloverridesBox->isChecked());
    s.setValue("dlloverrides_lineEdit_text", l.dlloverridesEdit->text());

    s.setValue("debug_checkbox_checked", l.debugBox->isChecked());
    s.setValue("debug_lineEdit_text", l.debugEdit->text());

    s.endGroup();
}
#endif

#include "runprogramdialog.moc"
