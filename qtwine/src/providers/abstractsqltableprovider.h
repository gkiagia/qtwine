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
class QSqlRecord;

class AbstractSqlTableProvider : public QObject
{
    Q_OBJECT
public:
    AbstractSqlTableProvider(QObject *parent = 0) : QObject(parent) {}
    virtual ~AbstractSqlTableProvider();

    inline QSqlTableModel *model() const;
    inline bool exists(int id) const;

    /* The following functions convert between rows and IDs of the items.
     * They both return positive numbers including 0 on success and -1 on failure.
     */
    int idToRow(int id) const;
    int rowToId(int row) const;
    QSqlRecord recordById(int id) const;

protected:
    /*! Generates a unique identifier for an item.
     * It is used to generate IDs for new items. \a name
     * is the name of the item that we want to generate the id for.
     * The id is normally the hash of the name, but if two items have
     * the same name or two names have the same hash, then this function
     * uses qrand() until it finds a unique ID.
     */
    int generateId(const QString & name) const;
    void setModel(QSqlTableModel *model);

private:
    QSqlTableModel *m_sqlModel;
};

inline QSqlTableModel *AbstractSqlTableProvider::model() const
{
    return m_sqlModel;
}

inline bool AbstractSqlTableProvider::exists(int id) const
{
    return idToRow(id) != -1;
}

#endif
