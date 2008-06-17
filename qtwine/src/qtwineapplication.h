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
#ifndef QTWINEAPPLICATION_H
#define QTWINEAPPLICATION_H

#include <KUniqueApplication>
#include "providers/wineinstallationsprovider.h"
#include "providers/wineconfigurationsprovider.h"
#include "providers/shortcutsprovider.h"
class QtWineMainWindow;

#define qtwineApp static_cast<QtWineApplication*>(kapp)
#define installationsProvider (qtwineApp->wineInstallationsProvider())
#define configurationsProvider (qtwineApp->wineConfigurationsProvider())
#define shortcutsProvider (qtwineApp->programShortcutsProvider())


class QtWineApplication : public KUniqueApplication
{
public:
    QtWineApplication();
    ~QtWineApplication();
    int newInstance();

    WineInstallationsProvider *wineInstallationsProvider() const;
    WineConfigurationsProvider *wineConfigurationsProvider() const;
    ShortcutsProvider *programShortcutsProvider() const;

private:
    void initializeDatabase();
    void initializeProviders();
    void deleteProviders();
    void shutDownDatabase();

    QtWineMainWindow *m_mainWindow;
    WineInstallationsProvider *m_installationsProvider;
    WineConfigurationsProvider *m_configurationsProvider;
    ShortcutsProvider *m_shortcutsProvider;
};

#endif
