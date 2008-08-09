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
#ifndef WINEDLLOVERRIDESREQUESTER_H
#define WINEDLLOVERRIDESREQUESTER_H

#include "winedlloverrides.h"
#include <KHBox>
class WineDllOverridesRequesterPrivate;


class WineDllOverridesRequester : public KHBox
{
    Q_OBJECT
    Q_PROPERTY(QString dllOverridesString READ dllOverridesString WRITE setDllOverridesString USER true)
    Q_PROPERTY(WineDllOverrides dllOverrides READ dllOverrides WRITE setDllOverrides)
public:
    WineDllOverridesRequester(QWidget *parent = 0);
    ~WineDllOverridesRequester();

    QString dllOverridesString() const;
    void setDllOverridesString(const QString & overridesString);

    QtWine::WineDllOverrides dllOverrides() const;
    void setDllOverrides(const QtWine::WineDllOverrides & overrides);

Q_SIGNALS:
    void overridesChanged();

private:
    WineDllOverridesRequesterPrivate *const d;
    Q_PRIVATE_SLOT(d, void toolButtonClicked())
    Q_PRIVATE_SLOT(d, void lineEditTextChanged())
};

#endif
