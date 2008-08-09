/***************************************************************************
 *   Copyright (C) 2008 by George Kiagiadakis <gkiagia@users.sf.net>       *
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
#ifndef WINEDLLOVERRIDESEDIT_H
#define WINEDLLOVERRIDESEDIT_H

#include "winedlloverrides.h"
#include <QScrollArea>
template <class T1, class T2> class QPair;
class WineDllOverridesEditPrivate;


class WineDllOverridesEdit : public QScrollArea
{
    Q_OBJECT
    /* normally, the DllOverrides property should be the USER property
     * but in the program shortcut editor, we need to use a QDataWidgetMapper
     * to assign a string to this widget, so the USER property must be a QString.
     */
    Q_PROPERTY(QString dllOverridesString READ dllOverridesString WRITE setDllOverridesString USER true)
    Q_PROPERTY(WineDllOverrides dllOverrides READ dllOverrides WRITE setDllOverrides)
public:
    WineDllOverridesEdit(QWidget *parent = 0);
    ~WineDllOverridesEdit();

    QString dllOverridesString() const;
    void setDllOverridesString(const QString & overrides);

    QtWine::WineDllOverrides dllOverrides() const;
    void setDllOverrides(const QtWine::WineDllOverrides & overrides);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

protected:
    virtual void resizeEvent(QResizeEvent *event);

Q_SIGNALS:
    void overridesChanged();

private Q_SLOTS:
    void addEmptyLine();
    void addLine(const QPair<QString, QtWine::WineDllOverrides::DllOverrideType> & o);
    void removeLine(QWidget *line);
    void resizeCentralWidget();

private:
    WineDllOverridesEditPrivate *d;
};


#endif
