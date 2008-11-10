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
#ifndef _QTWINE_FILERUNNER_H
#define _QTWINE_FILERUNNER_H

#include "libqtwine_export.h"
#include "libqtwine_global.h"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

LIBQTWINE_BEGIN_NAMESPACE

class FileRunnerPlugin;

class LIBQTWINE_EXPORT FileRunner : public QObject
{
    Q_OBJECT
public:
    FileRunner();
    FileRunner(const QString & file);
    virtual ~FileRunner();

    void setFile(const QString & file);
    void setOption(const QString & name, const QVariant & value);

    QString lastError() const;

    typedef FileRunnerPlugin *(*PluginInstanceFunction)(FileRunner *parent);
    static void registerPlugin(PluginInstanceFunction func, const QStringList & supportedFileExtensions);

public Q_SLOTS:
    virtual bool run();

protected:
    FileRunnerPlugin *initPlugin(const QString & fileExtension);

    void setLastError(const QString & errorMessage);
    QVariant option(const QString & name) const;

private:
    friend class FileRunnerPlugin;
    class Private;
    Private *const d;
};

LIBQTWINE_END_NAMESPACE

#endif
