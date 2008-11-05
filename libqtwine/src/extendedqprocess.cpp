/*
    This file is based on kprocess.cpp from the KDE libraries.
    Copyright (C) 2007 Oswald Buddenhagen <ossi@kde.org>
    Copyright (C) 2008 George Kiagiadakis <gkiagia@users.sourceforge.net>

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
#include "private/extendedqprocess_p.h"

LIBQTWINE_BEGIN_NAMESPACE

ExtendedQProcess::ExtendedQProcess(QObject *parent)
    : QProcess(parent), d_ptr(new ExtendedQProcessPrivate(this))
{
    setAutoDeleteEnabled(true);
}

ExtendedQProcess::ExtendedQProcess(ExtendedQProcessPrivate *dd, QObject *parent)
    : QProcess(parent), d_ptr(dd)
{
    setAutoDeleteEnabled(true);
}

ExtendedQProcess::~ExtendedQProcess()
{
    delete d_ptr;
}

void ExtendedQProcess::start()
{
    Q_D(ExtendedQProcess);
    QProcess::start(d->prog, d->args);
}

int ExtendedQProcess::execute(int msecs)
{
    start();
    if (!waitForFinished(msecs)) {
        kill();
        waitForFinished(-1);
        return -2;
    }
    return (exitStatus() == QProcess::NormalExit) ? exitCode() : -1;
}

bool ExtendedQProcess::autoDeleteEnabled() const
{
    Q_D(const ExtendedQProcess);
    return d->autoDelete;
}

void ExtendedQProcess::setAutoDeleteEnabled(bool enabled)
{
    Q_D(ExtendedQProcess);
    d->autoDelete = enabled;

    if ( enabled )
        connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(_p_autoDeleteHandler()) );
    else
        disconnect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
                   this, SLOT(_p_autoDeleteHandler()) );
}

void ExtendedQProcessPrivate::_p_autoDeleteHandler()
{
    Q_Q(ExtendedQProcess);
    q->deleteLater();
}


#define DUMMYENV "_KPROCESS_DUMMY_="

QString ExtendedQProcess::getEnv(const QString & name) const
{
    QStringList env = environment();
    if (env.isEmpty()) {
        env = systemEnvironment();
        env.removeAll(QString::fromLatin1(DUMMYENV));
    }

    QString fname(name);
    fname.append('=');
    for (QStringList::Iterator it = env.begin(); it != env.end(); ++it)
        if ((*it).startsWith(fname))
            return QString(*it).remove(fname);
    return QString();
}

void ExtendedQProcess::setEnv(const QString & name, const QString & value, SetEnvMode mode)
{
    QStringList env = environment();
    if (env.isEmpty()) {
        env = systemEnvironment();
        env.removeAll(QString::fromLatin1(DUMMYENV));
    }

    QString fname(name);
    fname.append('=');
    for (QStringList::Iterator it = env.begin(); it != env.end(); ++it)
        if ((*it).startsWith(fname)) {
            //the variable exists, act as requested in mode
            switch(mode) {
                case KeepIfExists:
                    break;
                case ReplaceIfExists:
                    *it = fname.append(value);
                    break;
                case ListPrepend:
                {
                    int i = (*it).indexOf('=') + 1;
                    (*it).insert(i, value + ':');
                    break;
                }
                case ListAppend:
                    (*it).append(':').append(value);
                    break;
                default:
                    Q_ASSERT(false);
            }
            setEnvironment(env);
            return;
        }

    //the variable does not exist in the list, append it.
    env.append(fname.append(value));
    setEnvironment(env);
}

void ExtendedQProcess::unsetEnv(const QString & name)
{
    QStringList env = environment();
    if (env.isEmpty()) {
        env = systemEnvironment();
        env.removeAll(QString::fromLatin1(DUMMYENV));
    }
    QString fname(name);
    fname.append('=');
    for (QStringList::Iterator it = env.begin(); it != env.end(); ++it)
        if ((*it).startsWith(fname)) {
            env.erase(it);
            if (env.isEmpty())
                env.append(DUMMYENV);
            setEnvironment(env);
            return;
        }
}

void ExtendedQProcess::clearEnvironment()
{
    setEnvironment(QStringList() << QString::fromLatin1(DUMMYENV));
}

#undef DUMMYENV


QStringList ExtendedQProcess::program() const
{
    Q_D(const ExtendedQProcess);

    QStringList argv = d->args;
    argv.prepend(d->prog);
    return argv;
}

void ExtendedQProcess::setProgram(const QString & exe, const QStringList & arguments)
{
    Q_D(ExtendedQProcess);

    d->prog = exe;
    d->args = arguments;
}

void ExtendedQProcess::setProgram(const QStringList & argv)
{
    Q_D(ExtendedQProcess);

    Q_ASSERT( !argv.isEmpty() );
    d->args = argv;
    d->prog = d->args.takeFirst();
}

void ExtendedQProcess::clearProgram()
{
    Q_D(ExtendedQProcess);

    d->prog.clear();
    d->args.clear();
}

ExtendedQProcess & ExtendedQProcess::operator<<(const QString & arg)
{
    Q_D(ExtendedQProcess);

    if (d->prog.isEmpty())
        d->prog = arg;
    else
        d->args << arg;
    return *this;
}

ExtendedQProcess & ExtendedQProcess::operator<<(const QStringList & args)
{
    Q_D(ExtendedQProcess);

    if (d->prog.isEmpty())
        setProgram(args);
    else
        d->args << args;
    return *this;
}

#include "extendedqprocess.moc"

LIBQTWINE_END_NAMESPACE
