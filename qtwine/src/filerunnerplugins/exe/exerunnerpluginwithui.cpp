/*
    Copyright (C) 2008  George Kiagiadakis <gkiagia@users.sourceforge.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dialogs.h"
#include "../../qtwineapplication.h"
#include "qtwinepreferences.h"

#include "wineconfiguration.h"
#include "argumentslist.h"
#include "winedlloverrides.h"

#include <QMetaEnum>
#include <KLocalizedString>
#include <KDebug>

using namespace QtWine;

void ExeRunnerPluginWithUi::run()
{
    /* first, attempt to see what action we should take.
      if no action has been specified, ask the user for it. */
    QMetaEnum actionMetaEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("Action"));
    QVariant actionOption = option("action");
    Action action = NoAction;
    if ( !actionOption.isNull() && actionOption.canConvert<QByteArray>() )
        action = (Action) actionMetaEnum.keyToValue(option("action").toByteArray().constData());

    if ( action == NoAction ) {
        ExeRunnerActionsDialog *dlg = new ExeRunnerActionsDialog();
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
        connect(dlg, SIGNAL(actionSelected(ExeRunnerPluginWithUi::Action)),
                this, SLOT(slotActionSelected(ExeRunnerPluginWithUi::Action)));
    } else
        slotActionSelected(action);
}

void ExeRunnerPluginWithUi::slotActionSelected(Action action)
{
    /* now an action has been specified. execute it. */
    switch (action) {
        case NoAction:
            /* probably the user hit cancel in the action selection dialog */
            emit finished(FileRunner::Failure);
            break;
        case Run:
        {
            /* show a dialog with the details before executing,
              except if the user asked not to show it */
            if ( !option("noDialog").toBool() )
                showRunDialog();
            else
                runSuperClassImplementation(option("wineConfigurationId").toInt());
            break;
        }
        default:
            kDebug() << action;
            Q_ASSERT(false); //TODO implement me
    }
}

void ExeRunnerPluginWithUi::showRunDialog()
{
    RunProgramDialog *dlg = new RunProgramDialog();
    dlg->setExecutable(option("file").toString());
    dlg->setArguments(option("arguments").value<ArgumentsList>());
    dlg->setWorkingDirectory(option("workdir").toString());
    dlg->setWineDllOverrides(option("wineDllOverrides").value<WineDllOverrides>());
    //TODO wineDebugOptions
    dlg->setLogFile(option("logFile").toString());
    dlg->setRunInTerminal(option("runInTerminal").toBool());
    dlg->setIsConsoleApplication(option("isConsoleApplication").toBool());

    /* check to see if the user specified a configuration id,
      and if not, use the default configuration id */
    int configurationId = -1;
    QVariant v = option("wineConfigurationId");
    if ( !v.isNull() && v.canConvert<int>() )
        configurationId = v.toInt();
    else
        configurationId = QtWinePreferences::defaultWineConfiguration();

    dlg->setWineConfigurationId(configurationId);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
    connect(dlg, SIGNAL(finished(int)), this, SLOT(runDialogFinished(int)));
}

void ExeRunnerPluginWithUi::runDialogFinished(int status)
{
    if ( status == QDialog::Rejected ) {
        emit finished(FileRunner::Failure);
        return;
    }

    RunProgramDialog *dlg = qobject_cast<RunProgramDialog*>(sender());
    Q_ASSERT(dlg);

    setOption("arguments", dlg->arguments());
    setOption("workdir", dlg->workingDirectory());
    setOption("wineDllOverrides", QVariant::fromValue(dlg->wineDllOverrides())); //FIXME operator QVariant()
    setOption("logFile", dlg->logFile()); //FIXME that may be a KUrl !!!
    setOption("runInTerminal", dlg->runInTerminal());
    setOption("isConsoleApplication", dlg->isConsoleApplication());

    runSuperClassImplementation(dlg->wineConfigurationId());
}

void ExeRunnerPluginWithUi::runSuperClassImplementation(int configurationId)
{
    if (configurationId > 0) {
        WineConfiguration c = qtwineApp->wineConfigurationsModel()->configurationById(configurationId);
        if ( KDE_ISUNLIKELY(c.isInvalid()) ) {
            emit error(i18n("There is no wine configuration with this id"));
            emit finished(FileRunner::Failure);
            return;
        } else {
            setOption("wineConfiguration", QVariant::fromValue(c));
        }
    }

    WineExecutableRunnerPlugin::run();
}

#include "exerunnerpluginwithui.moc"
