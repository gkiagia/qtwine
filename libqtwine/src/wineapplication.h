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

/*! <p> This class describes a windows application that is going to
 * be run with wine. This includes the filename of the windows
 * executable, its arguments, its working directory, whether it
 * is a CUI or a GUI application, the WINEPREFIX and wine installation
 * that will be used to launch this app as well as the wine environment
 * variables WINEDEBUG and WINEDLLOVERRIDES. It is used in conjunction
 * with WineProcess to automatate the task of creating a valid KProcess
 * that will launch wine correctly.</p>
 * <p> There are two ways to use this class. You can either construct it
 * using the default constructor and provide the information needed using
 * the transformer methods, or construct it with the other constructor
 * that takes the executable and a WineConfiguration as arguments and
 * provide the executable's command line arguments with operator<<().</p>
 * <p> The necessary information that have to be given before a
 * WineApplication can be run, is the executable name and the
 * WineConfiguration that will be used.</p>
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT WineApplication
{
public:
	//constructors
	WineApplication();
	WineApplication(const QString & executable, const WineConfiguration & configuration);
	WineApplication(const WineApplication & other);
	~WineApplication();

	//query functions
	bool isInvalid() const;

	//operators
	WineApplication & operator<<(const QString & arg);
	WineApplication & operator<<(const QStringList & args);
	WineApplication & operator=(const WineApplication & other);

	//transformers
	void setApplication(const QString & executable,
			    const QStringList & arguments = QStringList());
	void setWorkingDirectory(const QString & dir);

	void setWinePrefix(const QString & winePrefixPath);
	void setWineInstallation(const WineInstallation & installation);
	void setWineDllOverrides(const WineDllOverrides & dllOverrides);
	void setWineDebugOptions(const WineDebugOptions & wineDebug);

	void setIsConsoleApplication(bool isCuiApp);
	void enableRunInDebugger(bool enable);
	//void enableVirtualDesktop(const QString & name, const QPair<int, int> & resolution);

	// accessors
	QString executable() const;
	QStringList arguments() const;
	QString workingDirectory() const;

	QString winePrefix() const;
	WineInstallation wineInstallation() const;
	WineDllOverrides wineDllOverrides() const;
	WineDebugOptions wineDebugOptions() const;

	bool isConsoleApplication() const;
	bool runsInDebugger() const;

private:
	QSharedDataPointer<WineApplicationData> d;
};

LIBQTWINE_EXPORT QDebug operator<<(QDebug dbg, const WineApplication & a);

LIBQTWINE_END_NAMESPACE

Q_DECLARE_METATYPE(LIBQTWINE_PREPEND_NAMESPACE(WineApplication))

#endif
