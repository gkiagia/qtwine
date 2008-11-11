/***************************************************************************
 *   Copyright (C) 2008 by George Kiagiadakis                              *
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
#include "wineapplication.h"

#include <QDebug>
#include "winedlloverrides.h"
#include "winedebugoptions.h"

LIBQTWINE_BEGIN_NAMESPACE

class WineApplicationData : public QSharedData
{
public:
    WineApplicationData();
    WineApplicationData(const WineApplicationData & other);
    ~WineApplicationData();

    //basic stuff
    QString *executable;
    QStringList *arguments;
    QString *workingDirectory;

    //wine environment variables
    QString *winePrefix;
    WineInstallation *wineInstallation;
    WineDllOverrides *wineDllOverrides;
    WineDebugOptions *wineDebugOptions;

    //qtwine misc
    bool isConsoleApplication;
};

WineApplicationData::WineApplicationData()
    : QSharedData()
{
    isConsoleApplication = false;
    executable = workingDirectory = winePrefix = NULL;
    arguments = NULL;
    wineInstallation = NULL;
    wineDllOverrides = NULL;
    wineDebugOptions = NULL;
}

WineApplicationData::WineApplicationData(const WineApplicationData & other)
    : QSharedData(other)
{
#define COPY(T, var) var = (other.var) ? new T(*other.var) : NULL;
    COPY(QString, executable);
    COPY(QStringList, arguments);
    COPY(QString, workingDirectory);
    COPY(QString, winePrefix);
    COPY(WineInstallation, wineInstallation);
    COPY(WineDllOverrides, wineDllOverrides);
    COPY(WineDebugOptions, wineDebugOptions);
    isConsoleApplication = other.isConsoleApplication;
#undef COPY
}

WineApplicationData::~WineApplicationData()
{
    delete executable;
    delete arguments;
    delete workingDirectory;
    delete winePrefix;
    delete wineInstallation;
    delete wineDllOverrides;
    delete wineDebugOptions;
}

    //constructors
WineApplication::WineApplication() : d(new WineApplicationData) {}

WineApplication::WineApplication(const QString & executable,
                                 const WineConfiguration & configuration)
    : d(new WineApplicationData)
{
    setApplication(executable);
    setWinePrefix(configuration.winePrefix());
    setWineInstallation(configuration.wineInstallation());
}

WineApplication::WineApplication(const WineApplication & other) : d(other.d) {}
WineApplication::~WineApplication() {}

    // query functions

bool WineApplication::isInvalid() const
{
    return  executable().isEmpty() or
        winePrefix().isEmpty() or
        wineInstallation().isInvalid();
}

    //operators

WineApplication & WineApplication::operator<<(const QString & arg)
{
    if ( !d->executable ) {
        d->executable = new QString;
        *d->executable = arg;
        return *this;
    }

    if ( !d->arguments )
        d->arguments = new QStringList;
    *d->arguments << arg;
    return *this;
}

WineApplication & WineApplication::operator<<(const QStringList & args)
{
    QStringList copy(args);
    if ( !d->executable ) {
        d->executable = new QString;
        *d->executable = copy.takeFirst();
    }

    if ( !d->arguments )
        d->arguments = new QStringList;
    *d->arguments << copy;
    return *this;
}

WineApplication & WineApplication::operator=(const WineApplication & other)
{
    d = other.d;
    return *this;
}

    //transformers

#define ASSIGN_VAR(T, d_var, var) \
    do { \
        if ( !d->d_var ) \
            d->d_var = new T; \
        *d->d_var = var; \
    } while(0)

void WineApplication::setApplication(const QString & executable, const QStringList & arguments)
{
    ASSIGN_VAR(QString, executable, executable);

    if (d->arguments)
        d->arguments->clear();

    if ( !arguments.isEmpty() )
        operator<<(arguments);
}

void WineApplication::setWorkingDirectory(const QString & dir)
{
    ASSIGN_VAR(QString, workingDirectory, dir);
}

void WineApplication::setWinePrefix(const QString & winePrefixPath)
{
    ASSIGN_VAR(QString, winePrefix, winePrefixPath);
}

void WineApplication::setWineInstallation(const WineInstallation & installation)
{
    ASSIGN_VAR(WineInstallation, wineInstallation, installation);
}

void WineApplication::setWineDllOverrides(const WineDllOverrides & dllOverrides)
{
    ASSIGN_VAR(WineDllOverrides, wineDllOverrides, dllOverrides);
}

void WineApplication::setWineDebugOptions(const WineDebugOptions & wineDebug)
{
    ASSIGN_VAR(WineDebugOptions, wineDebugOptions, wineDebug);
}

void WineApplication::setIsConsoleApplication(bool isCuiApp)
{
    d->isConsoleApplication = isCuiApp;
}


#undef ASSIGN_VAR


    // accessors

#define RETURN_VAR(T, var)  return (d->var) ? *d->var : T()

QString WineApplication::executable() const { RETURN_VAR(QString, executable); }

QStringList WineApplication::arguments() const { RETURN_VAR(QStringList, arguments); }

QString WineApplication::workingDirectory() const { RETURN_VAR(QString, workingDirectory); }

QString WineApplication::winePrefix() const { RETURN_VAR(QString, winePrefix); }

WineInstallation WineApplication::wineInstallation() const
{
    RETURN_VAR(WineInstallation, wineInstallation);
}

WineDllOverrides WineApplication::wineDllOverrides() const
{
    RETURN_VAR(WineDllOverrides, wineDllOverrides);
}

WineDebugOptions WineApplication::wineDebugOptions() const
{
    RETURN_VAR(WineDebugOptions, wineDebugOptions);
}

bool WineApplication::isConsoleApplication() const { return d->isConsoleApplication; }


#undef RETURN_VAR

QDebug operator<<(QDebug dbg, const WineApplication & a)
{
    dbg.nospace() << "(WineApplication, [executable: " << a.executable() << "], ";
    dbg.nospace() << "[arguments: " << a.arguments() << "], ";
    dbg.nospace() << "[workingDirectory: " << a.workingDirectory() << "], ";
    dbg.nospace() << "[winePrefix: " << a.winePrefix() << "], ";
    dbg.nospace() << "[wineInstallation: " << a.wineInstallation() << "], ";
    dbg.nospace() << "[wineDllOverrides: " << a.wineDllOverrides() << "], ";
    dbg.nospace() << "[wineDebugOptions: " << a.wineDebugOptions() << "], ";
    dbg.nospace() << "[isConsoleApplication: " << a.isConsoleApplication() << "] )";

    return dbg.space();
}

LIBQTWINE_END_NAMESPACE
