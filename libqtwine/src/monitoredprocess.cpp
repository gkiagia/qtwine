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
#include "private/monitoredprocess_p.h"
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <KDebug>
#include <KLocalizedString>
#include <KMessage>
#include <KGlobal>
#include <KComponentData>
#include <KAboutData>

LIBQTWINE_BEGIN_NAMESPACE

/* This class provides a thread-safe pointer to the OpenTerminalFn.
 * ### Can we use a static QAtomicPointer to do this job?
 */
class TerminalFunctionHelper {
public:
	TerminalFunctionHelper() {
		QMutexLocker l(&mutex);
		QIODevice *defaultOpenTerminalFn(const QString & title);
		function = defaultOpenTerminalFn;
	}

	MonitoredProcess::OpenTerminalFn get() {
		QMutexLocker l(&mutex);
		return function;
	}

	void set(MonitoredProcess::OpenTerminalFn func) {
		QMutexLocker l(&mutex);
		function = func;
	}

private:
	QMutex mutex;
	MonitoredProcess::OpenTerminalFn function;
};

K_GLOBAL_STATIC(TerminalFunctionHelper, terminalFunctionHelper)


/* This is a private slot that is called when the MonitoredProcess emits
 * the finished() signal and autoDelete is enabled. It is there to prevent
 * the terminal from closing automatically as the user may want to inspect
 * wine's output on the terminal after wine has finished.
 */
void MonitoredProcessPrivate::_p_autoDeleteHandler()
{
	Q_Q(MonitoredProcess);

	/* If the terminal is open, delete this process object when the terminal closes
	 * else respect autoDelete and delete this process object immediately. */
	if ( m_terminalDevice && m_terminalDevice->isOpen() )
		QObject::connect(m_terminalDevice, SIGNAL(aboutToClose()), q, SLOT(deleteLater()) );
	else
		q->deleteLater();
}

MonitoredProcess::MonitoredProcess(QObject *parent)
	: KProcess(parent), d_ptr(new MonitoredProcessPrivate(this))
{
	Q_D(MonitoredProcess);
	d->m_connector = new ProcessIOConnector(this);
}

MonitoredProcess::MonitoredProcess(MonitoredProcessPrivate *dd, QObject *parent)
	: KProcess(parent), d_ptr(dd)
{
	Q_D(MonitoredProcess);
	d->m_connector = new ProcessIOConnector(this);
}

MonitoredProcess::~MonitoredProcess()
{
	delete d_ptr;
}

QString MonitoredProcess::logFile(ProcessOutputChannel channel) const
{
	const Q_D(MonitoredProcess);
	if ( d->m_logFile[channel] )
		return d->m_logFile[channel]->fileName();
	else
		return QString();
}

void MonitoredProcess::setLogFile(const QString & fileName, ProcessOutputChannel channel)
{
	if ( fileName.isEmpty() ) {
		kDebug() << "log fileName is empty";
		return;
	}

	Q_D(MonitoredProcess);

	delete d->m_logFile[channel];
	QFile *log = new QFile(fileName, d->m_connector);
	d->m_logFile[channel] = log;

	if ( !log->open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
		KMessage::message(KMessage::Error, i18n("Could not open the log file."
			"Check that it (or its directory) has sufficient permissions."));
		delete log;
		d->m_logFile[channel] = NULL;
		return;
	}

	ProcessIOConnector::ProcessChannels c;
	if ( channel & StandardOutput ) c |= ProcessIOConnector::StandardOutput;
	if ( channel & StandardError ) c |= ProcessIOConnector::StandardError;

	d->m_connector->connectIODevice(log, c);
}

bool MonitoredProcess::autoDeleteEnabled() const
{
	const Q_D(MonitoredProcess);
	return d->m_autoDelete;
}

void MonitoredProcess::setAutoDeleteEnabled(bool enabled)
{
	Q_D(MonitoredProcess);
	d->m_autoDelete = enabled;

	if ( enabled )
		connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
			this, SLOT(_p_autoDeleteHandler()) );
	else
		disconnect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
			   this, SLOT(_p_autoDeleteHandler()) );
}

void MonitoredProcess::setOpenTerminalFunction(OpenTerminalFn termFunc)
{
	terminalFunctionHelper->set(termFunc);
}

void MonitoredProcess::startMonitored()
{
	start();
}

void MonitoredProcess::openTerminal(MonitoredProcess::ProcessOutputChannel channel)
{
	Q_D(MonitoredProcess);
	if ( d->m_terminalDevice ) {
		kDebug() << "Terminal is already open";
		return;
	}

	OpenTerminalFn newTerminal = terminalFunctionHelper->get();
	Q_ASSERT_X(newTerminal, Q_FUNC_INFO, "OpenTerminalFunction has not been set");

	QString title;
	if ( KGlobal::hasMainComponent() )
		title = KGlobal::mainComponent().aboutData()->appName();
	else
		title = i18n("<Unknown application>");

	QStringList p = program();
	if ( !p.isEmpty() ) {
		title.append(" - ");
		title.append(p.join(" "));
	}

	d->m_terminalDevice = newTerminal(title);
	if ( d->m_terminalDevice ) {
		ProcessIOConnector::ProcessChannels c = ProcessIOConnector::StandardInput;
		if ( channel & StandardOutput ) c |= ProcessIOConnector::StandardOutput;
		if ( channel & StandardError ) c |= ProcessIOConnector::StandardError;
		d->m_connector->connectIODevice(d->m_terminalDevice, c);
	}
}

#include "monitoredprocess.moc"

LIBQTWINE_END_NAMESPACE
