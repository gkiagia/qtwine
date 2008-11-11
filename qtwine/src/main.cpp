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
    options.add("e");
    options.add("execute", ki18n("Directly execute [file] if it is executable without showing a dialog"));

    options.add("c");
    options.add("configuration <name>", ki18n("Specify the name of the wine configuration to be"
                " used for launching [file]\nIf not specified, the default wine configuration as"
                " specified in qtwine's settings will be used"));

    options.add("w");
    options.add("workdir <directory>", ki18n("Specify the working directory where wine will be run into"
                " for launching [file]\nIf not specified, the current working directory will be used"));

    options.add("dlloverrides <overrides>", ki18n("Set the WINEDLLOVERRIDES environment variable to"
                " be &lt;overrides&gt;\nIf not specified, the WINEDLLOVERRIDES environment variable will"
                " be taken into account"), qgetenv("WINEDLLOVERRIDES") );

    options.add("winedebug <dbgoptions>", ki18n("Set the WINEDEBUG environment variable to be &lt;dbgoptions&gt;"
                "\nIf not specified, the WINEDEBUG environment variable will be taken into account"),
                 qgetenv("WINEDEBUG"));

    options.add("d");
    options.add("cui", ki18n("Use wineconsole to launch [file]."
                " Use this option if [file] is a CUI/DOS application"));

    options.add("t");
    options.add("terminal", ki18n("Show wine's output in a terminal when launching [file]"));

    options.add("l");
    options.add("log <filename>", ki18n("A log file to save wine's output when launching [file]"));

    options.add("!+[file <arguments>]", ki18n("A file to open (.exe, .msi, .bat or .reg). If [file] is a"
                " windows executable,\n&lt;arguments&gt; are passed to it when it is executed, otherwise"
                " &lt;arguments&gt; are ignored.\nIf [file] is not specified, qtwine's main window opens."));
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
    aboutData.setBugAddress("gkiagia@users.sourceforge.net");
    aboutData.addAuthor(ki18n("George Kiagiadakis"), ki18n("Lead developer, main programer"),
                        "gkiagia@users.sourceforge.net", "http://www.devreactor.org/gkiagia");
    aboutData.addCredit(ki18n("Peter Penz"), ki18n("Dolphin metabar author"), "peter.penz@gmx.at");

    KCmdLineArgs::init( argc, argv, &aboutData );
    setupCmdLineOptions();
    return QtWineApplication().exec();
}
