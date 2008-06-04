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
#ifndef _QTWINE_WINEAPPLICATION_H
#define _QTWINE_WINEAPPLICATION_H

#include "wineconfiguration.h"
#include <QtCore/QStringList>
class QDebug;

LIBQTWINE_BEGIN_NAMESPACE

class WineDllOverrides;
class WineDebugOptions;
class WineApplicationData;

/*! \class WineApplication wineapplication.h <qtwine/wineapplication.h>
 * \brief Describes a windows application that is to be run using wine.
 * \par
 * This class describes a windows application that is going to
 * be run with wine. This includes the filename of the windows
 * executable, its arguments, its working directory, whether it
 * is a CUI or a GUI application, the WINEPREFIX and wine installation
 * that will be used to launch this app as well as the wine environment
 * variables WINEDEBUG and WINEDLLOVERRIDES. This class is meant to be
 * used in conjunction with WineProcess to automatate the task of
 * creating a valid KProcess that will launch wine correctly.
 * \par
 * There are two ways to use this class. You can either construct it
 * using the default constructor and provide the information needed using
 * the transformer methods, or construct it with the other constructor
 * that takes the executable and a WineConfiguration as arguments and
 * provide the executable's command line arguments with operator<<().
 * \par
 * The necessary information that has to be given before a
 * WineApplication can be run, is the executable name and the
 * WineConfiguration that will be used.
 * \note This class uses implicit sharing. See the Qt documentation
 * for more details on implicit sharing.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT WineApplication
{
public:
	//constructors
	/*! Constructs an invalid WineApplication. \sa isInvalid() */
	WineApplication();

	/*! Creates a new WineApplication that represents the specified
	 * \a executable using the specified wine \a configuration.
	 */
	WineApplication(const QString & executable, const WineConfiguration & configuration);

	/*! Creates a new WineApplication object that is copy of \a other. */
	WineApplication(const WineApplication & other);

	/*! Destroys this WineApplication object. */
	~WineApplication();

	//query functions

	/*! Returns true if this WineApplication object is invalid,
	 * which means that it can not be used with WineProcess to launch
	 * a windows application. To be valid, a WineApplication object
	 * requires to have specified an executable and a wine configuration
	 * (winePrefix() and wineInstallation()).
	 */
	bool isInvalid() const;

	//operators

	/*! Appends a command line argument to the list of command line
	 * arguments that will be passed to the windows executable.
	 * If an executable has not been set yet, \a arg will be set as
	 * the executable and command line arguments will be left untouched.
	 */
	WineApplication & operator<<(const QString & arg);

	/*! \overload */
	WineApplication & operator<<(const QStringList & args);

	/*! Makes \a this a copy of \a other */
	WineApplication & operator=(const WineApplication & other);

	//transformers

	/*! Sets the executable name and command line arguments. */
	void setApplication(const QString & executable,
			    const QStringList & arguments = QStringList());

	/*! Sets the working directory of the windows application. */
	void setWorkingDirectory(const QString & dir);

	/*! Sets the WINEPREFIX environment variable for the instance of
	 * wine that will execute this application.
	 */
	void setWinePrefix(const QString & winePrefixPath);

	/*! Sets the installation of wine that will be used to run this app. */
	void setWineInstallation(const WineInstallation & installation);

	/*! Sets the WINEDLLOVERRIDES environment variable for the
	 * instance of wine that will execute this application.
	 */
	void setWineDllOverrides(const WineDllOverrides & dllOverrides);

	/*! Sets the WINEDEBUG environment variable for the instance of
	 * wine that will execute this application.
	 */
	void setWineDebugOptions(const WineDebugOptions & wineDebug);

	/*! Defines whether the windows application is a CUI
	 * (Console User Interface / DOS) application.
	 * For wine to run correctly such applications, they need
	 * to be run in the "wineconsole" terminal emulator, so,
	 * if \a isCuiApp is true, the application will be
	 * executed inside "wineconsole", otherwise it will be
	 * executed normally.
	 */
	void setIsConsoleApplication(bool isCuiApp);

	/*! Enables this application to be run inside winedbg.exe for debugging purposes.
	 * \todo There is work to do with this option. It may not work as expected.
	 */
	void enableRunInDebugger(bool enable);

	// TODO void enableVirtualDesktop(const QString & name, const QPair<int, int> & resolution);

	// accessors

	/*! Returns the name (or filename) of the windows executable
	 * that this object represents.
	 */
	QString executable() const;

	/*! Returns the command line arguments that are to be passed to the executable. */
	QStringList arguments() const;

	/*! Returns the path to the working directory of the windows application. */
	QString workingDirectory() const;

	/*! Returns the value that will be given to $WINEPREFIX. */
	QString winePrefix() const;

	/*! Returns the WineInstallation that will be used to launch this application. */
	WineInstallation wineInstallation() const;

	/*! Returns the value that will be given to $WINEDLLOVERRIDES. */
	WineDllOverrides wineDllOverrides() const;

	/*! Returns the value that will be given to $WINEDEBUG. */
	WineDebugOptions wineDebugOptions() const;

	/*! Returns true if this application is a Console User Interface (CUI) application. */
	bool isConsoleApplication() const;

	/*! Returns true if this application is to be run inside winedbg.exe. */
	bool runsInDebugger() const;

private:
	QSharedDataPointer<WineApplicationData> d;
};

/*! \relates WineApplication
 * This is the QDebug stream operator for WineApplication.
 * This allows you to use qDebug() or kDebug() to dump a WineApplication
 * object on the terminal for debugging purposes.
 */
LIBQTWINE_EXPORT QDebug operator<<(QDebug dbg, const WineApplication & a);

LIBQTWINE_END_NAMESPACE

Q_DECLARE_METATYPE(LIBQTWINE_PREPEND_NAMESPACE(WineApplication))

#endif
