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
	bool runInDebugger;
};

WineApplicationData::WineApplicationData()
	: QSharedData()
{
	isConsoleApplication = runInDebugger = false;
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
	runInDebugger = other.runInDebugger;
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
/*! Constructs an invalid WineApplication. \sa isValid() */
WineApplication::WineApplication() : d(new WineApplicationData) {}

/*! Creates a new WineApplication that launches the specified \a executable
 * using the specified \a winePrefix and wine \a installation.
 */
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

/*! Returns true if this WineApplication object is invalid,
 * which means that it can not be used with WineProcess to launch
 * a windows application. To be valid, a WineApplication object
 * requires to have specified an executable and a wine configuration
 * (winePrefix() and wineInstallation()).
 */
bool WineApplication::isInvalid() const
{
	return  executable().isEmpty() or
		winePrefix().isEmpty() or
		wineInstallation().isInvalid();
}

	//operators

/*! Appends a command line argument to the list of command line
 * arguments that will be passed to the windows executable.
 * If an executable has not been set yet, \a arg will be set as
 * the executable and command line arguments will be left untouched.
 */
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

/*! \overload */
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

/*! Makes \a this a copy of \a other */
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

/*! Sets the executable name and command line arguments. */
void WineApplication::setApplication(const QString & executable, const QStringList & arguments)
{
	ASSIGN_VAR(QString, executable, executable);

	if (d->arguments)
		d->arguments->clear();

	if ( !arguments.isEmpty() )
		operator<<(arguments);
}

/*! Sets the working directory of the windows application. */
void WineApplication::setWorkingDirectory(const QString & dir)
{
	ASSIGN_VAR(QString, workingDirectory, dir);
}

/*! Sets the WINEPREFIX environment variable for the instance of
 * wine that will execute this application.
 */
void WineApplication::setWinePrefix(const QString & winePrefixPath)
{
	ASSIGN_VAR(QString, winePrefix, winePrefixPath);
}

/*! Sets the Installation of wine that will be used to run this app. */
void WineApplication::setWineInstallation(const WineInstallation & installation)
{
	ASSIGN_VAR(WineInstallation, wineInstallation, installation);
}

/*! Sets the WINEDLLOVERRIDES environment variable for the instance of
 * wine that will execute this application.
 */
void WineApplication::setWineDllOverrides(const WineDllOverrides & dllOverrides)
{
	ASSIGN_VAR(WineDllOverrides, wineDllOverrides, dllOverrides);
}

/*! Sets the WINEDEBUG environment variable for the instance of
 * wine that will execute this application.
 */
void WineApplication::setWineDebugOptions(const WineDebugOptions & wineDebug)
{
	ASSIGN_VAR(WineDebugOptions, wineDebugOptions, wineDebug);
}

/*! Defines whether the windows application is a CUI (command-line/DOS)
 * application. If \a isCuiApp is true (the default value), the application
 * will be executed inside "wineconsole", otherwise it will be executed
 * normally.
 */
void WineApplication::setIsConsoleApplication(bool isCuiApp)
{
	d->isConsoleApplication = isCuiApp;
}

/*! Enables this application to be run inside winedbg.exe for debugging purposes. */
void WineApplication::enableRunInDebugger(bool enable)
{
	d->runInDebugger = enable;
}

#undef ASSIGN_VAR


	// accessors

#define RETURN_VAR(T, var)  return (d->var) ? *d->var : T()

/*! Returns the name (or filename) of the windows executable to be run. */
QString WineApplication::executable() const { RETURN_VAR(QString, executable); }

/*! Returns the command line arguments that are to be passed to the executable. */
QStringList WineApplication::arguments() const { RETURN_VAR(QStringList, arguments); }

/*! Returns the path to the working directory of the windows application. */
QString WineApplication::workingDirectory() const { RETURN_VAR(QString, workingDirectory); }

/*! Returns the value that will be given to $WINEPREFIX. */
QString WineApplication::winePrefix() const { RETURN_VAR(QString, winePrefix); }

/*! Returns the WineInstallation that will be used to launch this app. */
WineInstallation WineApplication::wineInstallation() const
{
	RETURN_VAR(WineInstallation, wineInstallation);
}

/*! Returns the value that will be given to $WINEDLLOVERRIDES. */
WineDllOverrides WineApplication::wineDllOverrides() const
{
	RETURN_VAR(WineDllOverrides, wineDllOverrides);
}

/*! Returns the value that will be given to $WINEDEBUG. */
WineDebugOptions WineApplication::wineDebugOptions() const
{
	RETURN_VAR(WineDebugOptions, wineDebugOptions);
}

/*! Returns true if this application is a Console User Interface (CUI) application. */
bool WineApplication::isConsoleApplication() const { return d->isConsoleApplication; }

/*! Returns true if this application is to be run inside winedbg.exe. */
bool WineApplication::runsInDebugger() const { return d->runInDebugger; }

#undef RETURN_VAR

/*! \relates WineApplication
 * This is the QDebug stream operator for WineApplication.
 * This allows you to use qDebug() or kDebug() to dump a WineApplication
 * object on the terminal for debugging purposes.
 */
QDebug operator<<(QDebug dbg, const WineApplication & a)
{
	dbg.nospace() << "(WineApplication, [executable: " << a.executable() << "], ";
	dbg.nospace() << "[arguments: " << a.arguments() << "], ";
	dbg.nospace() << "[workingDirectory: " << a.workingDirectory() << "], ";
	dbg.nospace() << "[winePrefix: " << a.winePrefix() << "], ";
	dbg.nospace() << "[wineInstallation: " << a.wineInstallation() << "], ";
	dbg.nospace() << "[wineDllOverrides: " << a.wineDllOverrides() << "], ";
	dbg.nospace() << "[wineDebugOptions: " << a.wineDebugOptions() << "], ";
	dbg.nospace() << "[isConsoleApplication: " << a.isConsoleApplication() << "], ";
	dbg.nospace() << "[runsInDebugger: " << a.runsInDebugger() << "] )";

	return dbg.space();
}

LIBQTWINE_END_NAMESPACE
