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
#include "wineinstallation.h"
#include "extendedqprocess.h"

#include <QFile>
#include <QDir>
#include <QRegExp>
#include <QDebug>
#include <QVariant>

LIBQTWINE_BEGIN_NAMESPACE

class WineInstallationData : public QSharedData
{
public:
    WineInstallationData();
    WineInstallationData(const WineInstallationData & other);
    ~WineInstallationData();

    QString *prefix;
    QString *wineLoader;
    QString *wineServer;
    QString *wineDllPath;
};

WineInstallationData::WineInstallationData()
    : QSharedData()
{
    prefix = wineLoader = wineServer = wineDllPath = NULL;
}

WineInstallationData::WineInstallationData(const WineInstallationData & other)
    : QSharedData(other)
{
    prefix = (other.prefix) ? new QString(*other.prefix) : NULL;
    wineLoader = (other.wineLoader) ? new QString(*other.wineLoader) : NULL;
    wineServer = (other.wineServer) ? new QString(*other.wineServer) : NULL;
    wineDllPath = (other.wineDllPath) ? new QString(*other.wineDllPath) : NULL;
}

WineInstallationData::~WineInstallationData()
{
    delete prefix;
    delete wineLoader;
    delete wineServer;
    delete wineDllPath;
}


//// HELPERS ////

inline QString defaultWineLoader(QString prefix)
{
    if ( prefix.isEmpty() )
        return prefix; // return QString();

    if ( prefix.endsWith('/') )
        prefix.chop(1);

    if ( QFile::exists(prefix + "/lib/wine/wine.bin") )
        return prefix.append("/lib/wine/wine.bin");
    else
        return prefix.append("/bin/wine");
}

inline QString defaultWineServer(QString prefix)
{
    if ( prefix.isEmpty() )
        return prefix; // return QString();

    if ( prefix.endsWith('/') )
        prefix.chop(1);

    if ( QFile::exists(prefix + "/lib/wine/wineserver") )
        return prefix.append("/lib/wine/wineserver");
    else
        return prefix.append("/bin/wineserver");
}

inline QString defaultWineDllPath(QString prefix)
{
    if ( prefix.isEmpty() )
        return prefix; // return QString();

    if ( prefix.endsWith('/') )
        prefix.chop(1);

    return prefix.append("/lib/wine");
}

inline QString getWineVersion(const QString & wineloader)
{
    if ( wineloader.isEmpty() ) {
        qDebug() << "no wineloader specified";
        return QString();
    }

    ExtendedQProcess wine;
    wine.closeReadChannel(QProcess::StandardError);
    wine.setProgram(wineloader);
    wine << "--version";
    if ( wine.execute(5000) != 0 ) { // 5 secs is more than enough imho
        qCritical() << "Cannot start wine. KProcess error string:" << wine.errorString();
        return QString();
    } else {
        QString output(wine.readAllStandardOutput());
        QRegExp regexp("([0-9]+(\\.[0-9]+){1,2}(-\\w+){0,1})");
        if ( regexp.indexIn(output) == -1 ) {
            qDebug() << "Could not determine wine's version";
            return QString();
        } else
            return regexp.cap(0);
    }
}



//////// PUBLIC API /////////


/*static*/
WineInstallation WineInstallation::findWineInPath()
{
    QStringList env = QProcess::systemEnvironment();
    QString pathString = env.at( env.indexOf(QRegExp("^PATH=.*")) );
    pathString.remove(0, 5); //remove the starting "PATH="
    QStringList path = pathString.split(':', QString::SkipEmptyParts);

    foreach( QString binDir, path ) {
        QDir prefix(binDir);
        prefix.cdUp();
        if ( QFile::exists( defaultWineLoader(prefix.path()) ) )
            return WineInstallation(prefix.path());
    }

    return WineInstallation();
}


WineInstallation::WineInstallation()
    : d(new WineInstallationData)
{
}

WineInstallation::WineInstallation(const QString & prefix)
    : d(new WineInstallationData)
{
    setPrefix(prefix);
}

WineInstallation::WineInstallation(const WineInstallation & other) : d(other.d) {}
WineInstallation::~WineInstallation() {}

WineInstallation & WineInstallation::operator=(const WineInstallation & other)
{
    d = other.d;
    return *this;
}


bool WineInstallation::isInvalid() const
{
    return d->prefix == NULL;
}

QString WineInstallation::elementPath(const QString & element) const
{
    QString p = prefix();

    if ( element.startsWith('/') && p.endsWith('/') )
        p.chop(1);
    else if ( !element.startsWith('/') && !p.endsWith('/') )
        p.append('/');

    return p.append(element);
}

QString WineInstallation::wineVersion() const
{
    return getWineVersion(wineLoader());
}


QString WineInstallation::prefix() const
{
    return (d->prefix) ? *d->prefix : QString();
}

void WineInstallation::setPrefix(const QString & path)
{
    if ( !d->prefix )
        d->prefix = new QString;
    *d->prefix = path;
}



QString WineInstallation::wineDllPath() const
{
    return (d->wineDllPath) ? *d->wineDllPath : defaultWineDllPath(prefix());
}

void WineInstallation::setWineDllPath(const QString & path)
{
    if ( !d->wineDllPath )
        d->wineDllPath = new QString;
    *d->wineDllPath = path;
}



QString WineInstallation::wineLoader() const
{
    return (d->wineLoader) ? *d->wineLoader : defaultWineLoader(prefix());
}

void WineInstallation::setWineLoader(const QString & loader)
{
    if ( !d->wineLoader )
        d->wineLoader = new QString;
    *d->wineLoader = loader;
}



QString WineInstallation::wineServer() const
{
    return (d->wineServer) ? *d->wineServer : defaultWineServer(prefix());
}

void WineInstallation::setWineServer(const QString & server)
{
    if ( !d->wineServer )
        d->wineServer = new QString;
    *d->wineServer = server;
}

WineInstallation::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QDebug operator<<(QDebug dbg, const WineInstallation & i)
{
    dbg.nospace() << "(WineInstallation, [prefix: " << i.prefix() << "], ";
    dbg.nospace() << "[wineLoader: " << i.wineLoader() << "], ";
    dbg.nospace() << "[wineServer: " << i.wineServer() << "], ";
    dbg.nospace() << "[wineDllPath: " << i.wineDllPath() << "] )";

    return dbg.space();
}

LIBQTWINE_END_NAMESPACE
