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
#include "executablerequester.h"
#include "../qtwineapplication.h"
#include "wineconfiguration.h"
#include <KDebug>
#include <KFileDialog>

ExecutableRequester::ExecutableRequester(QWidget *parent)
    : KUrlRequester(parent)
{
    setMode(KFile::File | KFile::LocalOnly | KFile::ExistingOnly);
    setFilter("*.exe *.EXE *.exe.so *.EXE.SO|Wine executables (*.exe, *.exe.so)\n"
              "*.bat *.BAT|Windows batch files (*.bat)\n"
              "*.msi *.MSI|Windows installer packages (*.msi)\n"
              "*|All files");
    connect(this, SIGNAL(openFileDialog(KUrlRequester*)), this, SLOT(configureFileDialog()) );
}

void ExecutableRequester::setWineConfiguration(const QtWine::WineConfiguration & cfg)
{
    if ( KDE_ISLIKELY(!cfg.isInvalid()) )
        m_fileDialogStartPath = cfg.winePrefix() + "/dosdevices/c:/Program Files";
    else
        kDebug() << "Invalid WineConfiguration";
}

void ExecutableRequester::setWineConfiguration(int configurationsModelRow)
{
    QtWine::WineConfiguration c = qtwineApp->wineConfigurationsModel()->configurationByModelRow(configurationsModelRow);
    setWineConfiguration(c);
}

void ExecutableRequester::configureFileDialog()
{
    fileDialog()->setUrl(m_fileDialogStartPath);
}

#include "executablerequester.moc"
