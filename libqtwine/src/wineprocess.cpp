/***************************************************************************
 *   Copyright (C) 2007-2008 by George Kiagiadakis                         *
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
#include "wineprocess.h"
#include <QtGlobal>
#include <QFileInfo>
#include <KDebug>
#include "winedebugoptions.h"
#include "winedlloverrides.h"
#include "private/wineerrordetector_p.h"
#include "private/monitoredprocess_p.h"

LIBQTWINE_BEGIN_NAMESPACE

class WineProcessPrivate : public MonitoredProcessPrivate
{
public:
    WineProcessPrivate(WineProcess *qq)
        : MonitoredProcessPrivate(qq), m_errDetector(NULL) {}
    virtual ~WineProcessPrivate() {}

    WineErrorDetector *m_errDetector;
};


WineProcess::WineProcess(QObject *parent)
    : MonitoredProcess(new WineProcessPrivate(this), parent)
{
}

/*! Constructs a new WineProcess that is going to run the
 * WineApplication \a app and has \a parent as a parent.
 */
WineProcess::WineProcess(const WineApplication & app, QObject *parent)
    : MonitoredProcess(new WineProcessPrivate(this), parent)
{
    setWineApplication(app);
}

WineProcess::~WineProcess() {}

void WineProcess::setWineApplication(const WineApplication & app)
{
    if ( app.isInvalid() ) {
        kWarning() << "Called with an invalid WineApplication argument. This will fail!";
        return;
    }

    WineInstallation installation = app.wineInstallation();
    setProgram(installation.wineLoader());

    //setup argv
    if ( app.isConsoleApplication() )
        *this << "wineconsole.exe" << "--backend=user";

    *this << app.executable();
    *this << app.arguments();

    //setup environment
    setEnv("WINEPREFIX", app.winePrefix());
    setEnv("WINELOADER", installation.wineLoader());
    setEnv("WINESERVER", installation.wineServer());
    setEnv("WINEDLLPATH", installation.wineDllPath());
    setEnv("WINEDEBUG", app.wineDebugOptions());
    setEnv("WINEDLLOVERRIDES", app.wineDllOverrides());
    setEnv("PATH", installation.prefix() + "/bin", ListPrepend);

    //TODO check which other OSes use dyld (or maybe set both vars on all OSes?)
#ifdef Q_OS_DARWIN
    const char *ld_lib_path = "DYLD_LIBRARY_PATH";
#else
    const char *ld_lib_path = "LD_LIBRARY_PATH";
#endif
    setEnv(ld_lib_path, installation.prefix() + "/lib", ListPrepend);

    //setup working directory
    if ( !app.workingDirectory().isEmpty() )
        setWorkingDirectory(app.workingDirectory());
    else
        //FIXME what about winelib tools? is this working?
        setWorkingDirectory( QFileInfo(app.executable()).absolutePath() );
}

/*! Sets the environment variable \a variable to have the value \a value.
 * This environment variable is added to the wine's environment variable list.
 * \a mode is useful when the \a variable is a semi-colon separated list of values
 * (like $PATH) and defines whether the \a value will be prepended or appended
 * in the list. The default mode is to replace the value of the \a variable
 * with \a value without checking if it is a list.
 */
void WineProcess::setEnv(const QString & variable, const QString & value, SetEnvMode mode)
{
    if ( mode == Keep || mode == Replace ) {
        KProcess::setEnv(variable, value, static_cast<bool>(mode));
        return;
    }

    QStringList env = environment();
    if ( env.isEmpty() )
        env = systemEnvironment();

    QString var(variable);
    var.append('=');

    for (QStringList::Iterator it = env.begin(); it != env.end(); ++it) {
        if ((*it).startsWith(var)) {
            int i;
            switch(mode) {
                case ListPrepend:
                    i = (*it).indexOf('=') + 1;
                    (*it).insert(i, value + ':');
                    break;
                case ListAppend:
                    (*it).append(':').append(value);
                    break;
                default:
                    Q_ASSERT(false);
            }
            setEnvironment(env);
            return;
        }
    }

    env << var.append(value);
    setEnvironment(env);
}

bool WineProcess::wineHasError() const
{
    const Q_D(WineProcess);
    return (d->m_errDetector) ? !d->m_errDetector->lastError().isEmpty() : false;
}

QString WineProcess::lastWineError() const
{
    const Q_D(WineProcess);
    return (d->m_errDetector) ? d->m_errDetector->lastError() : QString();
}

void WineProcess::startMonitored()
{
    Q_D(WineProcess);
    d->m_errDetector = new WineErrorDetector(d->m_connector);
    d->m_connector->connectIODevice(d->m_errDetector, ProcessIOConnector::StandardError);
    connect(d->m_errDetector, SIGNAL(errorDetected(QString)), SIGNAL(wineError(QString)) );

    MonitoredProcess::startMonitored();
}

#include "wineprocess.moc"

LIBQTWINE_END_NAMESPACE
