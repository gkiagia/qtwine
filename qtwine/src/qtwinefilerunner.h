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
#ifndef QTWINEFILERUNNER_H
#define QTWINEFILERUNNER_H

#include "filerunner.h"
class KUrl;

class QtWineFileRunner : public QtWine::FileRunner
{
    Q_OBJECT
public:
    QtWineFileRunner();
    QtWineFileRunner(const QString & file);
    QtWineFileRunner(const KUrl & file);
    virtual ~QtWineFileRunner();

    void setFile(const KUrl & file);

    static void registerDefaultPlugins();

private slots:
    void showErrorMessage(const QString & message, FileRunner::ErrorSeverity severity);
};

#endif
