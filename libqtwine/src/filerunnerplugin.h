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
#ifndef _QTWINE_FILERUNNERPLUGIN_H
#define _QTWINE_FILERUNNERPLUGIN_H

#include "filerunner.h"

LIBQTWINE_BEGIN_NAMESPACE

class WineProcess;
class WineApplication;

class LIBQTWINE_EXPORT FileRunnerPlugin : public QObject
{
    Q_OBJECT
public:
    virtual ~FileRunnerPlugin() {}

public Q_SLOTS:
    virtual void run() = 0;

Q_SIGNALS:
    void error(const QString & message, FileRunner::ErrorSeverity severity = FileRunner::Important);
    void finished(FileRunner::FinishStatus status);

protected:
    FileRunnerPlugin(FileRunner *parent);
    QVariant option(const QString & name) const;
    void setOption(const QString & name, const QVariant & value);
};


/*! A FileRunnerPlugin that supports running windows executables (.exe).
 * Supported options:
 * \li ArgumentsList arguments
 * \li QString workingDirectory
 * \li WineConfiguration wineConfiguration (required)
 * \li WineDllOverrides wineDllOverrides
 * \li WineDebugOptions wineDebugOptions
 * \li bool isConsoleApplication
 * \li bool runInTerminal
 * \li QString logFile
 */
class LIBQTWINE_EXPORT WineExecutableRunnerPlugin : public FileRunnerPlugin
{
    Q_OBJECT
    Q_CLASSINFO("FileRunner Filetype", "extension/exe")
public:
    WineExecutableRunnerPlugin(FileRunner *parent) : FileRunnerPlugin(parent) {}

public Q_SLOTS:
    virtual void run();

protected:
    virtual WineProcess *initializeWineProcess();

private Q_SLOTS:
    void processFinished();
    void processError();
};

/*! A FileRunnerPlugin that supports merging windows registry files (.reg)
 * in the registry of some wineprefix.
 * Supported options:
 * \li WineConfiguration wineConfiguration (required)
 */
class LIBQTWINE_EXPORT RegistryFileRunnerPlugin : public FileRunnerPlugin
{
    Q_OBJECT
    Q_CLASSINFO("FileRunner Filetype", "extension/reg")
public:
    RegistryFileRunnerPlugin(FileRunner *parent) : FileRunnerPlugin(parent) {}

public Q_SLOTS:
    virtual void run();
};

LIBQTWINE_END_NAMESPACE

#endif
