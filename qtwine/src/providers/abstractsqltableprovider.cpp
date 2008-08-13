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

#include <KMessage>
#include <KLocalizedString>
#include <KRandom>

#include <QHash>
#include <QSqlError>
#include <QSqlRecord>

#include <climits>

AbstractSqlTableProvider::~AbstractSqlTableProvider()
{
	delete m_sqlModel;
}

int AbstractSqlTableProvider::idToRow(int id) const
{
    Q_ASSERT(m_sqlModel);
    int column = 0;
    QModelIndex start = m_sqlModel->index(0, column);
    QModelIndexList results = m_sqlModel->match(start, Qt::DisplayRole, QVariant(QString::number(id)) );
    return (results.size() > 0) ? results.at(0).row() : -1;
}

int AbstractSqlTableProvider::rowToId(int row) const
{
    Q_ASSERT(m_sqlModel);
    return m_sqlModel->record(row).value("id").toInt();
}

QSqlRecord AbstractSqlTableProvider::recordById(int id) const
{
    Q_ASSERT(m_sqlModel);
    return m_sqlModel->record(idToRow(id));
}

/*! Generates a unique identifier for an item.
 * It is used to generate IDs for new items. \a name
 * is the name of the item that we want to generate the id for.
 * The id is normally the hash of the name, but if two items have
 * the same name or two names have the same hash, then this function
 * uses qrand() until it finds a unique ID.
 */
int AbstractSqlTableProvider::generateId(const QString & name) const
{
    uint unsigned_id = qHash(name);

    //We take out the MS bit so that the id is always in the range 0 <= id <= INT_MAX
    int id = unsigned_id & INT_MAX; // that should take out the MS bit

    if ( id == 1 or exists(id) ) {
        do
            id = KRandom::random() & INT_MAX;
        while( id == 1 or exists(id) );
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
