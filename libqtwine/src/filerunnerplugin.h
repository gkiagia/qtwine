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
    virtual bool run() = 0;

Q_SIGNALS:
    void finished();

protected:
    FileRunnerPlugin(FileRunner *parent);
    QVariant option(const QString & name) const;
    void setLastError(const QString & errorMessage);
};

#ifdef MAKE_LIBQTWINE_LIB
//plugin instance functions used internally
FileRunnerPlugin *init_WineExecutableRunnerPlugin(FileRunner*);
FileRunnerPlugin *init_RegistryFileRunnerPlugin(FileRunner*);
#endif

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
public:
    virtual bool run();

protected:
    virtual WineProcess *initializeWineProcess();

    WineExecutableRunnerPlugin(FileRunner *parent);
#ifdef MAKE_LIBQTWINE_LIB
    friend FileRunnerPlugin *init_WineExecutableRunnerPlugin(FileRunner*);
#endif
};

/*! A FileRunnerPlugin that supports merging windows registry files (.reg)
 * in the registry of some wineprefix.
 * Supported options:
 * \li WineConfiguration wineConfiguration (required)
 */
class LIBQTWINE_EXPORT RegistryFileRunnerPlugin : public FileRunnerPlugin
{
    Q_OBJECT
public:
    virtual bool run();

protected:
    RegistryFileRunnerPlugin(FileRunner *parent);
#ifdef MAKE_LIBQTWINE_LIB
    friend FileRunnerPlugin *init_RegistryFileRunnerPlugin(FileRunner*);
#endif
};

LIBQTWINE_END_NAMESPACE

#endif
