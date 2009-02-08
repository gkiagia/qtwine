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
#include "registryfilerunnerplugin2.h"
#include "regfilemergedialog.h"
#include "../../qtwineapplication.h"
#include "qtwinepreferences.h"

#include "wineconfiguration.h"

#include <KLocalizedString>

using namespace QtWine;

void RegistryFileRunnerPlugin2::run()
{
    /* check to see if the user specified a configuration id */
    int configurationId = -1;
    QVariant v = option("wineConfigurationId");
    if ( !v.isNull() && v.canConvert<int>() ) {
        configurationId = v.toInt();
    }

    if ( !option("noDialog").toBool() ) {

        /* if no confuration id has been specified, we must obey the defaults */
        if ( configurationId == -1 )
            configurationId = QtWinePreferences::defaultWineConfiguration();

        RegfileMergeDialog *dlg = new RegfileMergeDialog(option("file").toString());
        dlg->setConfigurationId(configurationId);
        connect(dlg, SIGNAL(finished(int)), this, SLOT(mergeDialogFinished(int)));
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
    } else
        /* if the user doesn't want a dialog, we directly start the
         * implementation of the superclass but we set the configuration
         * first, if it has been specified by id. if configurationId is -1,
         * the superclass will run without any changes in its behavior.
         */
        runSuperClassImplementation(configurationId);
}

void RegistryFileRunnerPlugin2::mergeDialogFinished(int status)
{
    /* if the dialog was accepted (ok clicked), we merge the file in
     * the registry by using the superclass implementation, else we abort
     * with failure status (but with no error)
     */
    if ( status == QDialog::Accepted ) {
        RegfileMergeDialog *dlg = qobject_cast<RegfileMergeDialog*>(sender());
        Q_ASSERT(dlg);
        runSuperClassImplementation(dlg->configurationId());
    } else
        emit finished(FileRunner::Failure);
}

void RegistryFileRunnerPlugin2::runSuperClassImplementation(int configurationId)
{
    if (configurationId > 0) {
        WineConfiguration c = qtwineApp->wineConfigurationsModel()->configurationById(configurationId);
        if ( KDE_ISUNLIKELY(c.isInvalid()) ) {
            emit error(i18n("There is no wine configuration with this id"));
            emit finished(FileRunner::Failure);
            return;
        } else {
            setOption("wineConfiguration", c);
        }
    }

    RegistryFileRunnerPlugin::run();
}

#include "registryfilerunnerplugin2.moc"
