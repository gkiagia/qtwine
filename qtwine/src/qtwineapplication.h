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
#include <QPointer>
#include "models/wineinstallationsmodel.h"
#include "models/wineconfigurationsmodel.h"
#include "models/shortcutsmodel.h"
class QtWineMainWindow;

#define qtwineApp static_cast<QtWineApplication*>(kapp)
//#define installationsModel (qtwineApp->wineInstallationsModel())
//#define configurationsModel (qtwineApp->wineConfigurationsModel())
//#define shortcutsModel (qtwineApp->programShortcutsModel())


class QtWineApplication : public KUniqueApplication
{
    Q_OBJECT
public:
    QtWineApplication();
    ~QtWineApplication();
    int newInstance();

    WineInstallationsModel *wineInstallationsModel() const;
    WineConfigurationsModel *wineConfigurationsModel() const;
    ShortcutsModel *programShortcutsModel() const;

public slots:
    void showPreferencesDialog();

signals:
    void preferencesChanged();

private:
    void initializeDatabase();
    void initializeModels();
    void deleteModels();
    void shutDownDatabase();

    QPointer<QtWineMainWindow> m_mainWindow;
    WineInstallationsModel *m_installationsModel;
    WineConfigurationsModel *m_configurationsModel;
    ShortcutsModel *m_shortcutsModel;
};

#endif
