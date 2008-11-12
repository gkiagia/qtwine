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
#include "filerunnerplugin.h"
#include "wineprocess.h"
#include "winedlloverrides.h"
#include "winedebugoptions.h"
#include "argumentslist.h"
#include "regedit.h"

#include <QTextStream>
#include <QTemporaryFile>

LIBQTWINE_BEGIN_NAMESPACE

#define DECLARE_FILE_RUNNER_PLUGIN_COMMON(CLASS) \
    CLASS::CLASS(FileRunner *parent) : FileRunnerPlugin(parent) {} \
    FileRunnerPlugin * init_##CLASS(FileRunner *parent) \
    { \
        return new CLASS(parent); \
    }

FileRunnerPlugin::FileRunnerPlugin(FileRunner *parent) : QObject(parent) {}

QVariant FileRunnerPlugin::option(const QString & name) const
{
    return static_cast<FileRunner*>(parent())->option(name);
}

void FileRunnerPlugin::setLastError(const QString & errorMessage)
{
    static_cast<FileRunner*>(parent())->setLastError(errorMessage);
}


DECLARE_FILE_RUNNER_PLUGIN_COMMON(WineExecutableRunnerPlugin)

bool WineExecutableRunnerPlugin::run()
{
    QVariant v;
    WineApplication app;
    app.setApplication(option("file").toString());

    v = option("wineConfiguration");
    if ( !v.isNull() && v.canConvert<WineConfiguration>() ) {
        WineConfiguration c = v.value<WineConfiguration>();
        if ( c.isInvalid() ) {
            setLastError("Invalid WineConfiguration specified");
            return false;
        }
        app.setWineConfiguration(c);
    } else {
        setLastError("No WineConfiguration specified");
        return false;
    }

    v = option("arguments");
    if ( !v.isNull() && v.canConvert<ArgumentsList>() )
        app << v.value<ArgumentsList>();

    v = option("workingDirectory");
    if ( !v.isNull() && v.canConvert<QString>() )
        app.setWorkingDirectory(v.value<QString>());

    v = option("wineDllOverrides");
    if ( !v.isNull() && v.canConvert<WineDllOverrides>() )
        app.setWineDllOverrides(v.value<WineDllOverrides>());

    v = option("wineDebugOptions");
    if ( !v.isNull() && v.canConvert<WineDebugOptions>() )
        app.setWineDebugOptions(v.value<WineDebugOptions>());

    v = option("isConsoleApplication");
    if ( !v.isNull() && v.canConvert<bool>() )
        app.setIsConsoleApplication(v.value<bool>());

    WineProcess *process = initializeWineProcess();
    process->setWineApplication(app);

    v = option("runInTerminal");
    if ( !v.isNull() && v.canConvert<bool>() && v.value<bool>())
        process->openTerminal();

    v = option("logFile");
    if ( !v.isNull() && v.canConvert<QString>() ) {
        QString log = v.value<QString>();
        process->setLogFile(log);
    }

    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SIGNAL(finished()));
    process->startMonitored();
    return true;
}

WineProcess *WineExecutableRunnerPlugin::initializeWineProcess()
{
    return new WineProcess(this);
}

DECLARE_FILE_RUNNER_PLUGIN_COMMON(RegistryFileRunnerPlugin)

bool RegistryFileRunnerPlugin::run()
{
    QFile regfile( option("file").toString() );
    if ( !regfile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        setLastError(tr("Could not open the specified file"));
        return false;
    }

    QTemporaryFile utf8File;
    if ( !utf8File.open() ) {
        setLastError(tr("Could not create a temporary file (maybe the disk is full?)"));
        return false;
    }

    // we have to do this because windows regedit 5.0 or later uses utf16 for exported .reg files
    // and wine's regedit cannot read them in utf16 format
    QTextStream ts(&regfile);
    while ( !ts.atEnd() ) {
        utf8File.write( ts.readLine().toUtf8() );
        utf8File.write("\n");
    }

    QString utf8FileName = utf8File.fileName();
    regfile.close();
    utf8File.close();

    WineConfiguration c;
    QVariant v = option("wineConfiguration");
    if ( !v.isNull() && v.canConvert<WineConfiguration>() ) {
        c = v.value<WineConfiguration>();
        if ( c.isInvalid() ) {
            setLastError("Invalid WineConfiguration specified");
            return false;
        }
    } else {
        setLastError("No WineConfiguration specified");
        return false;
    }

    if ( RegEdit::importRegFile(utf8FileName, c) ) {
        emit finished();
        return true;
    } else {
        setLastError("Could not start wine");
        return false;
    }
}

#include "filerunnerplugin.moc"

LIBQTWINE_END_NAMESPACE
