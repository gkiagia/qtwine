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
#include "metatextlabel.h"

#include <KGlobalSettings>
#include <KSqueezedTextLabel>

#include <QHBoxLayout>
#include <QFormLayout>

MetaTextLabel::MetaTextLabel(QWidget* parent) :
    QWidget(parent),
    m_lines(0),
    m_layout(0)
{
}

MetaTextLabel::~MetaTextLabel()
{
}

void MetaTextLabel::clear()
{
    if (m_layout != 0) {
        m_layout->removeWidget(m_lines);
    }

    delete m_lines;
    m_lines = 0;

    delete m_layout;
    m_layout = 0;
}

void MetaTextLabel::add(const QString& labelText, const QString& infoText)
{
    if (m_lines == 0) {
        Q_ASSERT(m_layout == 0);

        m_lines = new QWidget(this);
        m_layout = new QHBoxLayout(this);
        m_layout->addWidget(m_lines);
        m_formLayout = new QFormLayout(m_lines);
        m_formLayout->setLabelAlignment(Qt::AlignRight);
        m_lines->show();
    }

    QLabel* label = new QLabel(labelText, m_lines);
    label->setFont(KGlobalSettings::smallestReadableFont());
    label->setAlignment(Qt::AlignRight | Qt::AlignTop);
    label->setWordWrap(true);

    QPalette palette = label->palette();
    QColor foreground = palette.color(QPalette::Foreground);
    foreground.setAlpha(128);
    palette.setColor(QPalette::Foreground, foreground);
    label->setPalette(palette);

    QLabel *info = 0;
    if (infoText.contains(' '))
        info = new QLabel(infoText, m_lines);
    else
        info = new KSqueezedTextLabel(infoText, m_lines);
    info->setFont(KGlobalSettings::smallestReadableFont());
    info->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    info->setWordWrap(true);

    m_formLayout->addRow(label, info);
}

#include "metatextlabel.moc"
