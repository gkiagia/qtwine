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

/*! This class extends KProcess and offers some extra functionality
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
	Q_PROPERTY(bool autoDelete READ autoDeleteEnabled WRITE setAutoDeleteEnabled)
	Q_DECLARE_PRIVATE(MonitoredProcess)
public:
	enum ProcessOutputChannelFlags {
		StandardOutput = 0x1,
		StandardError = 0x2,
		BothStdOutAndErr = StandardOutput | StandardError
	};
	Q_DECLARE_FLAGS(ProcessOutputChannel, ProcessOutputChannelFlags)

	explicit MonitoredProcess(QObject *parent = 0);
	virtual ~MonitoredProcess();

	QString logFile(ProcessOutputChannel channel = BothStdOutAndErr) const;
	void setLogFile(const QString & fileName, ProcessOutputChannel channel = BothStdOutAndErr);

	bool autoDeleteEnabled() const;
	void setAutoDeleteEnabled(bool enabled);

	typedef QIODevice *(*OpenTerminalFn)(const QString & title);
	static void setOpenTerminalFunction(OpenTerminalFn termFunc);

public Q_SLOTS:
	virtual void startMonitored();
	void openTerminal(ProcessOutputChannel channel = BothStdOutAndErr);

protected:
	MonitoredProcess(MonitoredProcessPrivate *dd, QObject *parent); //!< \internal
	MonitoredProcessPrivate *const d_ptr; //!< \internal

private:
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
