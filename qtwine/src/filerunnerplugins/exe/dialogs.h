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
#ifndef DIALOGS_H
#define DIALOGS_H

#include <KDialog>
#include "exerunnerpluginwithui.h"
class KUrlRequester;
class KSqueezedTextLabel;
class WineDllOverridesRequester;
class KLineEdit;
class QComboBox;
class QCheckBox;

namespace QtWine {
    class ArgumentsList;
    class WineDllOverrides;
}

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class RunProgramDialog : public KDialog
{
    Q_OBJECT
public:
    RunProgramDialog(QWidget *parent = 0);

    QString executable() const;
    void setExecutable(const QString & url);

    QtWine::ArgumentsList arguments() const;
    void setArguments(const QtWine::ArgumentsList & args);

    QString workingDirectory() const;
    void setWorkingDirectory(const QString & dir);

    int wineConfigurationId() const;
    void setWineConfigurationId(int id);

    QtWine::WineDllOverrides wineDllOverrides() const;
    void setWineDllOverrides(const QtWine::WineDllOverrides overrides);

    KUrl logFile() const;
    void setLogFile(const KUrl & logfile);

    bool runInTerminal() const;
    void setRunInTerminal(bool enabled);

    bool isConsoleApplication() const;
    void setIsConsoleApplication(bool enabled);

private:
    KSqueezedTextLabel *m_executableLabel;
    KLineEdit *m_argumentsEdit;
    KUrlRequester *m_workdirRequester;
    QComboBox *m_configComboBox;
    WineDllOverridesRequester *m_dllOverridesRequester;
    KUrlRequester *m_logfileRequester;
    QCheckBox *m_terminalBox;
    QCheckBox *m_wineconsoleBox;
};

class ExeRunnerActionsDialog : public KDialog
{
    Q_OBJECT
public:
    ExeRunnerActionsDialog(QWidget *parent = 0);

signals:
    void actionSelected(ExeRunnerPluginWithUi::Action action);

private slots:
    virtual void reject();
    void slotMapped(int who);
};

#endif
