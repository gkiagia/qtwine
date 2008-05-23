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
#ifndef ABSTRACTSQLTABLEPROVIDER_H
#define ABSTRACTSQLTABLEPROVIDER_H

#include <QSqlTableModel>

class AbstractSqlTableProvider : public QObject
{
	Q_OBJECT
public:
	AbstractSqlTableProvider() {}
	virtual ~AbstractSqlTableProvider();

	inline QSqlTableModel *model() const;
	inline bool exists(uint id) const;
	QModelIndex find(uint id) const;

protected:
	uint generateId(const QString & name) const;
	//bool submitChanges(); TODO maybe remove, not used
	void setModel(QSqlTableModel *model);

private:
	QSqlTableModel *m_sqlModel;
};

inline QSqlTableModel *AbstractSqlTableProvider::model() const
{
	return m_sqlModel;
}

inline bool AbstractSqlTableProvider::exists(uint id) const
{
	return find(id).isValid();
}

#endif
