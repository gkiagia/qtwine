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
#include "abstractsqltableprovider.h"
#include <QHash>
#include <KMessage>
#include <KLocalizedString>
#include <QSqlError>
#include <ctime>

AbstractSqlTableProvider::~AbstractSqlTableProvider()
{
	delete m_sqlModel;
}

QModelIndex AbstractSqlTableProvider::find(uint id) const
{
	Q_ASSERT(m_sqlModel);
	int column = 0;
	QModelIndex start = m_sqlModel->index(0, column);
	QModelIndexList results = m_sqlModel->match(start, Qt::DisplayRole, QVariant(QString::number(id)) );
	return (results.size() > 0) ? results.at(0) : QModelIndex();
}

/*! Generates a unique identifier for an item.
 * It is used to generate IDs for new items. \a name
 * is the name of the item that we want to generate the id for.
 * The id is normally the hash of the name, but if two items have
 * the same name or two names have the same hash, then this function
 * uses qrand() until it finds a unique ID.
 */
uint AbstractSqlTableProvider::generateId(const QString & name) const
{
	uint id = qHash(name);

	if ( id == 1 or exists(id) ) {
		qsrand(time(NULL));
		do {
			id = qrand();
		} while( id == 1 or exists(id) );
	}
	
	return id;
}

void AbstractSqlTableProvider::setModel(QSqlTableModel *model)
{
	Q_ASSERT(model);
	m_sqlModel = model;
	m_sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
	m_sqlModel->select();
}

#if 0
bool AbstractActionProvider::submitChanges()
{
	Q_ASSERT(m_sqlModel);
	if ( KDE_ISUNLIKELY( !m_sqlModel->submitAll() ) ) {
		KMessage::message(KMessage::Error,
				  i18n("Could not submit changes to the database."
				  " SQL error message was: %1")
				  .arg(m_sqlModel->lastError().text()) );
		return false;
	}
	return true;
}
#endif

#include "abstractsqltableprovider.moc"
