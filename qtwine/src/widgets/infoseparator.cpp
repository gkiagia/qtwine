/***************************************************************************
 *   Copyright (C) 2008 by Peter Penz <peter.penz@gmx.at>                  *
 *   Copyright (C) 2008 by George Kiagiadakis <gkiagia@users.sourceforge.net>*
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
#include "infoseparator.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionMenuItem>

InfoSeparator::InfoSeparator(QWidget* parent)
	: QWidget(parent)
{
	setMinimumSize(0, 8);
}

InfoSeparator::~InfoSeparator()
{
}

void InfoSeparator::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	QStyleOptionMenuItem option;
	option.initFrom(this);
	option.menuItemType = QStyleOptionMenuItem::Separator;
	style()->drawControl(QStyle::CE_MenuItem, &option, &painter, this);
}
