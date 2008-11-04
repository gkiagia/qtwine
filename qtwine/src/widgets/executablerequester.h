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
#ifndef EXECUTABLE_REQUESTER_H
#define EXECUTABLE_REQUESTER_H

#include <KUrlRequester>
namespace QtWine { class WineConfiguration; }

class ExecutableRequester : public KUrlRequester
{
    Q_OBJECT
public:
    ExecutableRequester(QWidget *parent = 0);

public slots:
    void setWineConfiguration(const QtWine::WineConfiguration & cfg);
    void setWineConfiguration(int configurationsModelRow);

private slots:
    void configureFileDialog();

private:
    KUrl m_fileDialogStartPath;
};

#endif
