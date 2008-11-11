/*
    Copyright (C) 2008  George Kiagiadakis <gkiagia@users.sourceforge.net>

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
#include "qtwinefilerunner.h"

#include <KGlobal>
#include <KMessageBox>
#include <KIO/NetAccess>

using namespace QtWine;

QtWineFileRunner::QtWineFileRunner()
    : FileRunner()
{
    KGlobal::ref();
}

QtWineFileRunner::QtWineFileRunner(const QString & file)
    : FileRunner(file)
{
    KGlobal::ref();
}

QtWineFileRunner::QtWineFileRunner(const KUrl & file)
    : FileRunner()
{
    KGlobal::ref();
    setFile(file);
}

QtWineFileRunner::~QtWineFileRunner()
{
    QString tmpFile = option("file").toString();
    KIO::NetAccess::removeTempFile(tmpFile);
    KGlobal::deref();
}

void QtWineFileRunner::setFile(const KUrl & file)
{
    QString tmpFile;
    if( KIO::NetAccess::download(file, tmpFile, 0)) {
        FileRunner::setFile(tmpFile);
    } else {
        KMessageBox::error(0, KIO::NetAccess::lastErrorString());
    }
}

bool QtWineFileRunner::run()
{
    if ( !FileRunner::run() ) {
        KMessageBox::error(0, lastError());
        return false;
    }
    return true;
}

#include "qtwinefilerunner.moc"
