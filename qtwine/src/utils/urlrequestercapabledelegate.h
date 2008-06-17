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
#ifndef URLREQUESTERCAPABLEDELEGATE_H
#define URLREQUESTERCAPABLEDELEGATE_H

#include <QSqlRelationalDelegate>

/*! Custom delegate that adds support for setting/getting data
 * to/from a KUrlRequester when the model provides only a QString for the url.
 * \author George Kiagiadakis <gkiagia@users.sourceforge.net>
 */
class UrlRequesterCapableDelegate : public QSqlRelationalDelegate
{
public:
        UrlRequesterCapableDelegate(QObject *parent = 0) : QSqlRelationalDelegate(parent) {}
        virtual void setEditorData(QWidget *editor, const QModelIndex & index) const;
        virtual void setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  const QModelIndex & index) const;
};

#endif
