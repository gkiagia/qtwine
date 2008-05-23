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
#ifndef METABAR_H
#define METABAR_H

#include <QWidget>
class QString;
class QIcon;
class MetaTextLabel;
class QLabel;

class MetaBar : public QWidget
{
	Q_OBJECT
public:
	MetaBar(QWidget *parent = 0);

	void setIcon(const QIcon & icon);
	void addInformationPair(const QString & label, const QString & value);
	void clearInformation();

private:
	QLabel *m_iconLabel;
	MetaTextLabel *m_metaTextLabel;
};

#endif
