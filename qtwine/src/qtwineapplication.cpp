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
#include "qtwineapplication.h"
#include "qtwinemainwindow.h"
#include "qtwinepreferences.h"
#include "ui_behaviorPreferences.h"

#include "wineprocess.h"
#include "winedlloverrides.h"
#include "winedebugoptions.h"

#include <QFile>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <KCmdLineArgs>
#include <KLocalizedString>
#include <KStandardDirs>
#include <KMessage>
#include <KConfigDialog>

QtWineApplication::QtWineApplication()
    : KUniqueApplication(), m_mainWindow(NULL)
{
    initializeDatabase();
    initializeModels();
}

QtWineApplication::~QtWineApplication()
{
    delete m_mainWindow;
    deleteModels();
    shutDownDatabase();
}


int QtWineApplication::newInstance()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if ( args->count() == 0 ) {
        /* No file was specified on the command line,
         * proceed with opening the main window. */
        if ( !m_mainWindow ) {
            m_mainWindow = new QtWineMainWindow();
            m_mainWindow->show();
        } else {
            m_mainWindow->show();
            m_mainWindow->raise();
            m_mainWindow->activateWindow();
        }
    } else { // we are launching the specified file
        if ( args->isSet("execute") ) {
            using namespace QtWine;
            WineConfiguration c;

            if ( args->isSet("configuration") ) {
                QModelIndex start = m_configurationsModel->index(0, m_configurationsModel->fieldIndex("name"));
                QModelIndexList results = m_configurationsModel->
                            match( start, Qt::DisplayRole, QVariant(args->getOption("configuration")) );
                if ( results.size() > 0 )
                    c = m_configurationsModel->configurationByModelRow(results.at(0).row());
                else {
                    KMessage::message(KMessage::Fatal, i18n("The specified wine configuration \"%1\""
                                        " does not exist").arg(args->getOption("configuration")) );
                    args->clear();
                    return 1;
                }
            } else {
                c = m_configurationsModel->configurationById(QtWinePreferences::defaultWineConfiguration());
            }

            WineApplication a(args->arg(0), c);
            for (int i=1; i<args->count(); i++)
                a << args->arg(i);
            a.setWorkingDirectory( args->isSet("workdir") ? args->getOption("workdir") : args->cwd() );
            a.setWineDllOverrides( WineDllOverrides(args->getOption("dlloverrides")) );
            a.setWineDebugOptions( WineDebugOptions(args->getOption("winedebug")) );
            a.setIsConsoleApplication( args->isSet("cui") );
            a.enableRunInDebugger( args->isSet("debug") );

            WineProcess *wine = new WineProcess(a);
            if ( args->isSet("log") )
                wine->setLogFile( args->makeURL(args->getOption("log").toLocal8Bit()).path() );
            if ( args->isSet("terminal") )
                wine->openTerminal();
            wine->setAutoDeleteEnabled(true);
            wine->startMonitored(); //FIXME needs KGlobal::ref()/deref()

        } else { // --noexecute
            kDebug() << "Executing apps without --execute is not supported yet";
            args->clear();
            return 1;
        }
    }

    args->clear();
    return 0;
}

void QtWineApplication::initializeDatabase()
{
    if ( !QSqlDatabase::isDriverAvailable("QSQLITE") ){
        KMessage::message(KMessage::Fatal,
                          i18n("The SQLite plugin for Qt is not installed. Please install it."));
        exit(1);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    QString dbFile = KStandardDirs::locateLocal("data", "qtwine/qtwine.db", true);
    db.setDatabaseName(dbFile);

    if ( !db.open() ) {
        KMessage::message(KMessage::Fatal, i18n("Could not load qtwine's database"));
        exit(1);
    }
}

void QtWineApplication::initializeModels()
{
    m_installationsModel = new WineInstallationsModel(this);
    m_configurationsModel = new WineConfigurationsModel(this);
    m_shortcutsModel = new ShortcutsModel(this);
}

void QtWineApplication::deleteModels()
{
    delete m_installationsModel;
    delete m_configurationsModel;
    delete m_shortcutsModel;
}

void QtWineApplication::shutDownDatabase()
{
    //we must retrieve the name before calling removeDatabase because
    //if the QSqlDatabase object that QSqlDatabase::database() constructs
    //exists, removeDatabase will output a warning.
    QString connectionName = QSqlDatabase::database().connectionName();
    QSqlDatabase::removeDatabase(connectionName);
}


class BehaviorPreferencesWidget : public QWidget, Ui::BehaviorPreferencesWidget {
public:
    BehaviorPreferencesWidget(QWidget *parent = 0) : QWidget(parent) {
        setupUi(this);
    }
};

void QtWineApplication::showPreferencesDialog()
{
    if ( KConfigDialog::showDialog( "qtwinepreferences" ) )
        return; 

    KConfigDialog* dialog = new KConfigDialog(0, "qtwinepreferences", QtWinePreferences::self()); 

    BehaviorPreferencesWidget* bPrefs = new BehaviorPreferencesWidget();
    dialog->addPage(bPrefs, i18n("Behavior"), "configure");
 
    connect( dialog, SIGNAL(settingsChanged(QString)), this, SIGNAL(preferencesChanged()) ); 

    dialog->show();
}

#include "qtwineapplication.moc"
