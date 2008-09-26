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
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <KPageDialog>
class KUrlRequester;
class ExecutableRequester;
class WineDllOverridesRequester;
class KLineEdit;
class QComboBox;
class QCheckBox;

/*!
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class Launcher : public KPageDialog
{
    Q_OBJECT
public:
    Launcher(QWidget *parent = 0);

public slots:
#if 0
    void setCommand(const QString & command);
    void setCommand(const QStringList & command);
#endif
    void accept();

private slots:
    void slotExecutableChanged(const KUrl & newUrl);

private:
    //void loadSession();
    //void storeSession();

    ExecutableRequester *m_executableRequester;
    KLineEdit *m_argumentsEdit;
    KUrlRequester *m_workdirRequester;
    QComboBox *m_configComboBox;
    WineDllOverridesRequester *m_dllOverridesRequester;
    KUrlRequester *m_logfileRequester;
    QCheckBox *m_terminalBox;
    QCheckBox *m_wineconsoleBox;
    QCheckBox *m_winedbgBox;
};

#endif
