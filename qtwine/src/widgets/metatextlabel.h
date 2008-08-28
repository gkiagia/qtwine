/***************************************************************************
 *   This file is part of the dolphin project. Modified for QtWine.        *
 *                                                                         *
 *   Copyright (C) 2008 by Peter Penz <peter.penz@gmx.at>                  *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef METATEXTLABEL_H
#define METATEXTLABEL_H

#include <QWidget>

class QHBoxLayout;
class QFormLayout;

/**
 * @brief Displays general meta in several lines.
 *
 * Each line contains a label and the the meta information.
 */
class MetaTextLabel : public QWidget
{
    Q_OBJECT

public:
    MetaTextLabel(QWidget* parent = 0);
    virtual ~MetaTextLabel();

    void clear();
    void add(const QString& labelText, const QString& infoText);

private:
    QWidget* m_lines;
    QHBoxLayout* m_layout;
    QFormLayout *m_formLayout;
};

#endif
