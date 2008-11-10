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
#include <QDebug>
#include <KGlobal>

LIBQTWINE_BEGIN_NAMESPACE

////// FACTORY //////

class FileRunnerPluginFactory
{
public:
    FileRunnerPluginFactory();

    void registerPlugin(FileRunner::PluginInstanceFunction func, const QStringList & extensions);
    FileRunner::PluginInstanceFunction pluginForExtension(const QString & extension);

private:
    QHash<QString, FileRunner::PluginInstanceFunction> m_hashMap;
};

K_GLOBAL_STATIC(FileRunnerPluginFactory, globalPluginFactory);

FileRunnerPluginFactory::FileRunnerPluginFactory()
{
    m_hashMap.insert("exe", init_WineExecutableRunnerPlugin);
    m_hashMap.insert("reg", init_RegistryFileRunnerPlugin);
}

void FileRunnerPluginFactory::registerPlugin(FileRunner::PluginInstanceFunction func, const QStringList & extensions)
{
    QStringList::const_iterator it;
    for(it = extensions.constBegin(); it != extensions.constEnd(); ++it)
        m_hashMap.insert((*it).toLower(), func);
}

FileRunner::PluginInstanceFunction FileRunnerPluginFactory::pluginForExtension(const QString & extension)
{
    return m_hashMap.value(extension.toLower(), NULL);
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

void FileRunner::setLastError(const QString & errorMessage)
{
    d->lastError = errorMessage;
}

//static
void FileRunner::registerPlugin(PluginInstanceFunction func, const QStringList & supportedFileExtensions)
{
    globalPluginFactory->registerPlugin(func, supportedFileExtensions);
}

FileRunnerPlugin *FileRunner::initPlugin(const QString & fileExtension)
{
    PluginInstanceFunction f = globalPluginFactory->pluginForExtension(fileExtension);
    if ( f == NULL ) {
        d->lastError = "No plugin registered for extension " + fileExtension;
        qWarning() << "FileRunner::initPlugin:" << d->lastError;
        return NULL;
    }

    FileRunnerPlugin *instance = f(this);
    if ( instance == NULL ) {
        d->lastError = "Failed to load plugin for extension " + fileExtension;
        qWarning() << "FileRunner::initPlugin:" << d->lastError;
        return NULL;
    }

    return instance;
}

bool FileRunner::run()
{
    if ( d->file.isEmpty() ) {
        d->lastError = "No file specified";
        qWarning() << "FileRunner::run:" << d->lastError;
        deleteLater();
        return false;
    }

    QRegExp extensionRegExp(".*\\.(\\w+)$");
    if ( extensionRegExp.indexIn(d->file) == -1 ) {
        d->lastError = "Could not determine the type of the specified file";
        qWarning() << "FileRunner::run:" << d->lastError;
        deleteLater();
        return false;
    }

    QString extension = extensionRegExp.cap(1);

    // workaround to make wine's tools load too.
    if ( extension == "so" && d->file.endsWith("exe.so", Qt::CaseInsensitive) )
        extension = "exe";

    FileRunnerPlugin *plugin = initPlugin(extension);
    if ( plugin == NULL ) {
        //warning has been already shown by initPlugin()
        deleteLater();
        return false;
    }

    if ( !plugin->run() ) {
        qWarning() << "FileRunner::run:" << "Plugin for extension" << extension << "returned error:" << d->lastError;
        deleteLater();
        return false;
    }

    connect(plugin, SIGNAL(finished()), this, SLOT(deleteLater()));
    return true;
}

#include "filerunner.moc"

LIBQTWINE_END_NAMESPACE
