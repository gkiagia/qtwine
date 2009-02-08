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
#include "filerunnerplugin.h"
#include <QHash>
#include <QStringList>
#include <QRegExp>
#include <QMetaClassInfo>
#include <QDebug>
#include <KGlobal>

LIBQTWINE_BEGIN_NAMESPACE

////// FACTORY //////

class FileRunnerPluginFactory
{
public:
    typedef FileRunnerPlugin *(*PluginInstanceFunction)(FileRunner *);

    void registerPlugin(PluginInstanceFunction func, const QStringList & types);
    PluginInstanceFunction pluginForType(const QString & type);

private:
    QHash<QString, PluginInstanceFunction> m_hashMap;
};

K_GLOBAL_STATIC(FileRunnerPluginFactory, globalPluginFactory);

void FileRunnerPluginFactory::registerPlugin(PluginInstanceFunction func, const QStringList & types)
{
    QStringList::const_iterator it;
    for(it = types.constBegin(); it != types.constEnd(); ++it)
        m_hashMap.insert(*it, func);
}

FileRunnerPluginFactory::PluginInstanceFunction FileRunnerPluginFactory::pluginForType(const QString & type)
{
    return m_hashMap.value(type, NULL);
}


////// RUNNER ///////

class FileRunner::Private
{
public:
    QString file;
    QHash<QString, QVariant> options;
    QString lastError;
};

FileRunner::FileRunner() : QObject(), d(new Private) {}

FileRunner::FileRunner(const QString & file)
    : QObject(), d(new Private)
{
    setFile(file);
}

FileRunner::~FileRunner()
{
    delete d;
}

void FileRunner::setFile(const QString & file)
{
    d->file = file;
    setOption("file", d->file);
}

QVariant FileRunner::option(const QString & name) const
{
    return d->options.value(name, QVariant());
}

void FileRunner::setOption(const QString & name, const QVariant & value)
{
    d->options.insert(name, value);
}

QString FileRunner::lastError() const
{
    return d->lastError;
}

void FileRunner::emitError(const QString & errorMessage, ErrorSeverity severity)
{
    d->lastError = errorMessage;
    qDebug() << "FileRunner::emitError" << errorMessage;
    emit error(errorMessage, severity);
}

//static
void FileRunner::registerPlugin(PluginInstanceFunction func, const QMetaObject & metaObject)
{
    QStringList fileTypes;
    for(int i=0; i < metaObject.classInfoCount(); ++i) {
        QMetaClassInfo ci = metaObject.classInfo(i);
        if ( !qstrcmp(ci.name(), "FileRunner Filetype") )
            fileTypes << ci.value();
    }
    globalPluginFactory->registerPlugin(func, fileTypes);
}

//static
void FileRunner::registerDefaultPlugins()
{
    FileRunner::registerPlugin<WineExecutableRunnerPlugin>();
    FileRunner::registerPlugin<RegistryFileRunnerPlugin>();
}

FileRunnerPlugin *FileRunner::initPlugin(const QString & fileType)
{
    PluginInstanceFunction f = globalPluginFactory->pluginForType(fileType);
    if ( f == NULL ) {
        emitError(tr("Unsupported file type \"%1\"").arg(fileType));
        return NULL;
    }

    FileRunnerPlugin *instance = f(this);
    Q_ASSERT(instance);
    return instance;
}

bool FileRunner::start()
{
    connect(this, SIGNAL(finished(FileRunner::FinishStatus)), this, SLOT(deleteLater()));

    if ( d->file.isEmpty() ) {
        emitError(tr("No file specified"));
        emit finished(Failure);
        return false;
    }

    QString fileType = determineFileType(d->file);
    if ( fileType.isEmpty() ) {
        emitError(tr("Could not determine the type of the specified file"));
        emit finished(Failure);
        return false;
    }

    FileRunnerPlugin *plugin = initPlugin(fileType);
    if ( plugin == NULL ) {
        emit finished(Failure);
        return false;
    }

    connect(plugin, SIGNAL(finished(FileRunner::FinishStatus)),
            this, SIGNAL(finished(FileRunner::FinishStatus)));
    connect(plugin, SIGNAL(error(QString, FileRunner::ErrorSeverity)),
            this, SLOT(emitError(QString, FileRunner::ErrorSeverity)));

    QMetaObject::invokeMethod(plugin, "run", Qt::QueuedConnection);
    return true;
}

QString FileRunner::determineFileType(const QString & file)
{
    // that's quite stupid implementation, it should find the mime type instead,
    // but it works for the needs of qtwine at least :)
    QRegExp extensionRegExp(".*\\.(\\w+)$");
    if ( extensionRegExp.indexIn(file) == -1 )
        return QString();

    QString extension = extensionRegExp.cap(1);
    extension = extension.toLower();

    // workaround to make wine's tools load too.
    if ( extension == "so" && file.endsWith("exe.so") )
        extension = "exe";

    return extension.prepend("extension/");
}

#include "filerunner.moc"

LIBQTWINE_END_NAMESPACE
