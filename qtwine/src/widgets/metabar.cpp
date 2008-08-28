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
#include "metabar.h"
#include "metatextlabel.h"
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include <KLocalizedString>

MetaBar::MetaBar(QWidget *parent)
	: QWidget(parent)
{
	//setupUi(this);
	m_iconLabel = new QLabel(this);
	m_iconLabel->setAlignment(Qt::AlignCenter);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);

	m_metaTextLabel = new MetaTextLabel(this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_iconLabel);
	layout->addWidget(separator);
	layout->addWidget(m_metaTextLabel);

	// ensure that widgets in the information side bar are aligned towards the top
	layout->addStretch(1);
}

void MetaBar::setIcon(const QIcon & icon)
{
	m_iconLabel->setPixmap(icon.pixmap(48, 48));
}

void MetaBar::addInformationPair(const QString & label, const QString & value)
{
	m_metaTextLabel->add(label + ':', value);
}

void MetaBar::clearInformation()
{
	m_metaTextLabel->clear();
}

#include "metabar.moc"
