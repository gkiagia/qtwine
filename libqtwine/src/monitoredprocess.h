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
#ifndef _QTWINE_MONITOREDPROCESS_H
#define _QTWINE_MONITOREDPROCESS_H

#include "libqtwine_export.h"
#include "libqtwine_global.h"
#include <KDE/KProcess>

LIBQTWINE_BEGIN_NAMESPACE

class MonitoredProcessPrivate;

/*! \class MonitoredProcess monitoredprocess.h <qtwine/monitoredprocess.h>
 * \brief Provides a Monitored KProcess.
 * \par
 * This class extends KProcess and offers some extra functionality
 * for monitoring the output of the process. This is completely
 * independent from wine stuff. Currently, this class offers
 * monitoring the output of the process in a terminal window
 * and logging the output to a file.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT MonitoredProcess : public KProcess
{
	Q_OBJECT
	Q_FLAGS(ProcessOutputChannel)

	/*! This property holds whether this class should be deleted
	 * automatically when the process has finished or not.
	 * \note When a terminal window is open and monitors the output of
	 * this process and autoDelete is enabled, the process will only be
	 * deleted when both the process has finished and the terminal window
	 * has been closed by the user.
	 */
	Q_PROPERTY(bool autoDelete READ autoDeleteEnabled WRITE setAutoDeleteEnabled)

public:
	enum ProcessOutputChannelFlags {
		StandardOutput = 0x1,
		StandardError = 0x2,
		BothStdOutAndErr = StandardOutput | StandardError
	};
	Q_DECLARE_FLAGS(ProcessOutputChannel, ProcessOutputChannelFlags)

	/*! Constructs a new MonitoredProcess object that has \a parent as a QObject parent. */
	explicit MonitoredProcess(QObject *parent = 0);

	/*! Destroys this MonitoredProcess object. */
	virtual ~MonitoredProcess();


	/*! Returns the filename of the log file that has been set
	 * for the specified \a channel or an empty string if no log file
	 * has been set for this channel.
	 */
	QString logFile(ProcessOutputChannel channel = BothStdOutAndErr) const;

	/*! Sets \a fileName to be the log file for the specified \a channel.
	 * You can set up to three different log files for a process, each one
	 * monitoring a different output channel of the process. Output channels
	 * are StandardOutput (stdout), StandardError (stderr) and BothStdOutAndErr
	 * (which combines stdout and stderr in one channel). If no channel is specified,
	 * the default is to monitor BothStdOutAndErr.
	 */
	void setLogFile(const QString & fileName, ProcessOutputChannel channel = BothStdOutAndErr);


	/*! \sa autoDelete */
	bool autoDeleteEnabled() const;

	/*! \sa autoDelete */
	void setAutoDeleteEnabled(bool enabled);


	/*! This is a typedef for a function that opens a terminal emulator window
	 * with the specified \a title as a window title and returns a QIODevice
	 * pointer that can be used to read and write data from/to the terminal.
	 * \sa setOpenTerminalFunction
	 */
	typedef QIODevice *(*OpenTerminalFn)(const QString & title);

	/*! Sets the OpenTerminalFn that will be used by the openTerminal method
	 * to open the terminal window. It is not necessary to provide such a
	 * function, as libqtwine already provides a default implementation.
	 * The default OpenTerminalFn of this library launches xterm running
	 * qtwine-terminal-helper inside and connects to qtwine-terminal-helper
	 * using a QLocalSocket so that data can be sent to and read from
	 * the terminal. \sa setDefaultTerminalApplication
	 */
	static void setOpenTerminalFunction(OpenTerminalFn termFunc);

public Q_SLOTS:
	/*! Starts the process and enables extra monitoring of the process' output.
	 * This function must be subclassed to enable extra monitoring. The default
	 * implementation only calls KProcess::start().
	 */
	virtual void startMonitored();

	/*! Opens a new terminal window using the OpenTerminalFn that has
	 * been set using setOpenTerminalFunction and shows on the terminal
	 * the output of this process for the specified \a channel.
	 * In addition, if the terminal function implementation supports it,
	 * the input from the terminal will be connected to the process' stdin,
	 * so that this class can even be used to launch an interactive console
	 * application.
	 * \note In contrast with the log file, you can only launch one terminal
	 * for a process, which will monitor only one of the three available channels.
	 * \todo Give more control to the user regarding the stdin part and maybe
	 * allow more terminals.
	 */
	void openTerminal(ProcessOutputChannel channel = BothStdOutAndErr);

protected:
	MonitoredProcess(MonitoredProcessPrivate *dd, QObject *parent); //!< \internal
	MonitoredProcessPrivate *const d_ptr; //!< \internal

private:
	Q_DECLARE_PRIVATE(MonitoredProcess)
	Q_PRIVATE_SLOT(d_func(), void _p_autoDeleteHandler())
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MonitoredProcess::ProcessOutputChannel)

/*! \relates MonitoredProcess
 * When you are using the default MonitoredProcess::OpenTerminalFn for
 * opening a terminal with MonitoredProcess, that function will open
 * a real terminal application where it will show the output of the process.
 * With this function, you can set which terminal application will be opened.
 * The default is "xterm".
 * \note The terminal application must support the -T and -e command line
 * arguments as they are used by xterm. Konsole 3.5.x is known to work.
 * \warning This function may change or may be removed in a future major version
 * of this library.
 */
LIBQTWINE_EXPORT void setDefaultTerminalApplication(const QString & str);

LIBQTWINE_END_NAMESPACE

#endif
