/*
    This file is based on kprocess.h from the KDE libraries.
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
#ifndef _QTWINE_EXTENDEDQPROCESS_H
#define _QTWINE_EXTENDEDQPROCESS_H

#include "libqtwine_global.h"
#include "libqtwine_export.h"
#include <QtCore/QProcess>

LIBQTWINE_BEGIN_NAMESPACE

class ExtendedQProcessPrivate;

/*! This is an extended version of QProcess that implements some of
 * the features of KProcess as well as autodeletion support and handling
 * environment variables that are lists (like $PATH).
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class LIBQTWINE_EXPORT ExtendedQProcess : public QProcess
{
    Q_OBJECT

    //FIXME this note below should be in MonitoredProcess
    /*! This property holds whether this class should be deleted
     * automatically when the process has finished or not.
     * \note When a terminal window is open and monitors the output of
     * this process and autoDelete is enabled, the process will only be
     * deleted when both the process has finished and the terminal window
     * has been closed by the user.
     */
    Q_PROPERTY(bool autoDelete READ autoDeleteEnabled WRITE setAutoDeleteEnabled)

public:
    enum SetEnvMode { KeepIfExists, ReplaceIfExists, ListPrepend, ListAppend };

    explicit ExtendedQProcess(QObject *parent = 0);

    virtual ~ExtendedQProcess();

    /*!
     * Start the process.
     *
     * \see QProcess::start(const QString &, const QStringList &, OpenMode)
     */
    virtual void start();

    /*!
     * Start the process, wait for it to finish, and return the exit code.
     *
     * This method is roughly equivalent to the sequence:
     * \code
     *   start();
     *   waitForFinished(msecs);
     *   return exitCode();
     * \endcode
     *
     * Unlike the other execute() variants this method is not static,
     * so the process can be parametrized properly and talked to.
     *
     * \param msecs time to wait for process to exit before killing it
     * \return -2 if the process could not be started, -1 if it crashed,
     *  otherwise its exit code
     */
    int execute(int msecs = -1);

    /*! \sa autoDelete */
    bool autoDeleteEnabled() const;

    /*! \sa autoDelete */
    void setAutoDeleteEnabled(bool enabled);


    QString getEnv(const QString & name) const;

    /*!
     * Adds the variable \p name to the process' environment.
     *
     * This function must be called before starting the process.
     *
     * \param name the name of the environment variable
     * \param value the new value for the environment variable
     * \param mode the action to do if the environment variable already exists
     * \see SetEnvMode
     */
    void setEnv(const QString & name, const QString & value, SetEnvMode mode = ReplaceIfExists);

    /*!
     * Removes the variable \p name from the process' environment.
     *
     * This function must be called before starting the process.
     *
     * \param name the name of the environment variable
     */
    void unsetEnv(const QString & name);

    /*!
     * Empties the process' environment.
     *
     * Note that LD_LIBRARY_PATH/DYLD_LIBRARY_PATH is automatically added
     * on *NIX.
     *
     * This function must be called before starting the process.
     */
    void clearEnvironment();


    /*!
     * Obtain the currently set program and arguments.
     *
     * \return a list, the first element being the program, the remaining ones
     *  being command line arguments to the program.
     */
    QStringList program() const;

    /*!
     * Set the program and the command line arguments.
     *
     * This function must be called before starting the process, obviously.
     *
     * \param exe the program to execute
     * \param arguments the command line arguments for the program,
     *   one per list element
     */
    void setProgram(const QString & exe, const QStringList & arguments = QStringList());

    /*!
     * \overload
     *
     * \param argv the program to execute and the command line arguments
     *   for the program, one per list element
     */
    void setProgram(const QStringList & argv);

    /*!
     * Clear the program and command line argument list.
     */
    void clearProgram();

    /*!
     * Append an element to the command line argument list for this process.
     *
     * If no executable is set yet, it will be set instead.
     *
     * For example, doing an "ls -l /usr/local/bin" can be achieved by:
     *  \code
     *  ExtendedQProcess p;
     *  p << "ls" << "-l" << "/usr/local/bin";
     *  ...
     *  \endcode
     *
     * This function must be called before starting the process, obviously.
     *
     * \param arg the argument to add
     * \return a reference to this ExtendedQProcess
     */
    ExtendedQProcess & operator<<(const QString & arg);

    /*!
     * \overload
     *
     * \param args the arguments to add
     * \return a reference to this ExtendedQProcess
     */
    ExtendedQProcess & operator<<(const QStringList & args);

protected:
    ExtendedQProcess(ExtendedQProcessPrivate *dd, QObject *parent); //!< \internal
    ExtendedQProcessPrivate *const d_ptr; //!< \internal

private:
    Q_DECLARE_PRIVATE(ExtendedQProcess)
    Q_PRIVATE_SLOT(d_func(), void _p_autoDeleteHandler())
};

LIBQTWINE_END_NAMESPACE

#endif
