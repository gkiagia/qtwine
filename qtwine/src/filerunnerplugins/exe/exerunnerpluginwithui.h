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
#ifndef EXERUNNERPLUGINWITHUI_H
#define EXERUNNERPLUGINWITHUI_H

#include "filerunnerplugin.h"

/*! A FileRunnerPlugin that supports running windows executables (.exe).
 * Supported actions:

 Action Run, CreateShortcut:
 * \li ArgumentsList arguments
 * \li QString workingDirectory
 * \li WineConfiguration wineConfiguration (required)
 * \li WineDllOverrides wineDllOverrides
 * \li WineDebugOptions wineDebugOptions
 * \li bool isConsoleApplication
 * \li bool runInTerminal
 * \li QString logFile

 Action Install:
    wineConfiguration (only if specified explicitly)
 */
class ExeRunnerPluginWithUi : public QtWine::WineExecutableRunnerPlugin
{
    Q_OBJECT
    Q_ENUMS(Action)
public:
    enum Action { NoAction, Run, Install, CreateShortcut };
    ExeRunnerPluginWithUi(QtWine::FileRunner *parent) : WineExecutableRunnerPlugin(parent) {}

public slots:
    virtual void run();

private slots:
    void slotActionSelected(ExeRunnerPluginWithUi::Action action);
    void runDialogFinished(int status);

private:
    void showRunDialog();
    void runSuperClassImplementation(int configurationId);
    int getWineConfigurationId(bool idMustExist);
};

#endif
