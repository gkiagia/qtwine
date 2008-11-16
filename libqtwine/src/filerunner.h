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
    Q_ENUMS(FinishStatus ErrorSeverity)
    Q_DISABLE_COPY(FileRunner)
public:
    enum FinishStatus {
        Failure = 0,
        Success = 1
    };

    enum ErrorSeverity {
        Important = 0,
        Critical = 1,
        ProgrammerError = 2
    };

    FileRunner();
    FileRunner(const QString & file);
    virtual ~FileRunner();

    QString lastError() const;

    void setFile(const QString & file);
    void setOption(const QString & name, const QVariant & value);

    template <class T>
    static void registerPlugin()
    {
        registerPlugin(defaultPluginInstanceFunction<T>, T::staticMetaObject);
    }

    static void registerDefaultPlugins();

Q_SIGNALS:
    void error(const QString & errorMessage, ErrorSeverity severity);
    void finished(FinishStatus status);

public Q_SLOTS:
    virtual bool start();

protected Q_SLOTS:
    void emitError(const QString & errorMessage, ErrorSeverity severity = Important);

protected:
    virtual QString determineFileType(const QString & file);
    FileRunnerPlugin *initPlugin(const QString & fileType);
    QVariant option(const QString & name) const;

private:
    typedef FileRunnerPlugin *(*PluginInstanceFunction)(FileRunner *);
    static void registerPlugin(PluginInstanceFunction func, const QMetaObject & metaObject);

    template <class T>
    static FileRunnerPlugin *defaultPluginInstanceFunction(FileRunner *runner)
    {
        return new T(runner);
    }

    friend class FileRunnerPlugin;
    class Private;
    Private *const d;
};

LIBQTWINE_END_NAMESPACE

#endif
