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
#ifndef QTWINESQLTABLEMODEL_H
#define QTWINESQLTABLEMODEL_H

#include <QSqlRelationalTableModel>

class QtWineSqlTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    QtWineSqlTableModel(QObject *parent = 0);

    inline bool exists(int id) const;

    /* The following functions convert between rows and IDs of the items.
     * They both return positive numbers including 0 on success and -1 on failure.
     */
    int idToRow(int id) const;
    int rowToId(int row) const;
    QSqlRecord recordById(int id) const;

    /*! Returns the integer value of an int field that has been related
     * with some text on another model (for example, the "wineinstallation"
     * field of the "wine_configurations" table).
     * \param row the row of this model for which we want to retrieve the
     * related field's value.
     * \param fieldName the name of the field in this model that has been
     * related to the other model.
     * \note This function is a workaround because doing model.record(row)
     * returns a QSqlRecord that cannot access the integer value of the related
     * field. It only gives access to the related text from the other model.
     */
    int relationId(int row, const char *fieldName);

protected:
    /*! Generates a unique identifier for an item.
     * It is used to generate IDs for new items. \a name
     * is the name of the item that we want to generate the id for.
     * The id is normally the hash of the name, but if two items have
     * the same name or two names have the same hash, then this function
     * uses qrand() until it finds a unique ID.
     */
    int generateId(const QString & name) const;
};

inline bool QtWineSqlTableModel::exists(int id) const
{
    return idToRow(id) != -1;
}

#endif
