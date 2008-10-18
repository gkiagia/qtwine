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
#include <KAboutData>
#include <KCmdLineArgs>
#include "qtwineapplication.h"

#define QTWINE_VERSION_STR "0.5-devel"

// HACK to abort from Q_ASSERT(), needed for any non-debug version of Qt <= 4.4.0
#if (QT_VERSION <= 0x040400)
# include <iostream>
# include <cstdlib>
static void qtMessageHandler(QtMsgType type, const char *msg)
{
    std::cerr << msg << std::endl;
    if ( type == QtFatalMsg )
        abort();
}
#endif


static inline void setupCmdLineOptions()
{
    KCmdLineOptions options;
    options.add("c");
    options.add("configuration <name/id>", ki18n("The name or internal ID of the wine"
                " configuration to be used for launching [file]."));

    options.add("i");
    options.add("installation <name/id>", ki18n("The name or internal ID of the wine"
                " installation to be used for launching [file]."));

    options.add("l");
    options.add("log <filename>", ki18n("A log file to save wine's output when launching [file]"));

    options.add("w");
    options.add("workdir <directory>", ki18n("The working directory where wine will"
                " be run into for launching [file]."));

    options.add("t");
    options.add("terminal", ki18n("Run wine in a terminal when launching [file]."));

    options.add("d");
    options.add("cuiapp", ki18n("Use wineconsole to launch [file]."
                " Use it if [file] is a CUI/DOS application."));

    options.add("s");
    options.add("silent", ki18n("Run in silent mode. No windows will appear and the specified"
                " action will be executed if possible.\nCan also be used for launching qtwine on"
                " the system tray only without showing the main window."));

    options.add("!+[file] <arguments>", ki18n("A file to open (.exe, .msi, .bat or .reg)."));

    options.add("", ki18n("If [file] is a windows executable, <arguments>"
                " are passed to it when it is executed, otherwise <arguments> are ignored."));
    KCmdLineArgs::addCmdLineOptions(options);
}


int main (int argc, char *argv[])
{
#if (QT_VERSION <= 0x040400)
    qInstallMsgHandler(qtMessageHandler);
#endif

    KAboutData aboutData( "qtwine", 0, ki18n("QtWine"), QTWINE_VERSION_STR,
                        ki18n("A full-featured GUI for wine."),
                        KAboutData::License_GPL,
                        ki18n("(C) 2006-2008, George Kiagiadakis") );
    aboutData.setOrganizationDomain("sourceforge.net");
    aboutData.setHomepage("http://qtwine.sourceforge.net");
    aboutData.setBugAddress("http://bugs.devreactor.org");
    aboutData.addAuthor(ki18n("George Kiagiadakis"), ki18n("Lead developer, main programer"),
                        "gkiagia@users.sourceforge.net", "http://www.devreactor.org/gkiagia");
    aboutData.addCredit(ki18n("Peter Penz"), ki18n("Dolphin metabar author"), "peter.penz@gmx.at");

    KCmdLineArgs::init( argc, argv, &aboutData );
    setupCmdLineOptions();
    return QtWineApplication().exec();
}
