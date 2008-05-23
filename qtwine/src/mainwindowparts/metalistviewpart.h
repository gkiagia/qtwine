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
#ifndef METALISTVIEWPART_H
#define METALISTVIEWPART_H

#include <KParts/Part>
#include <QList>
#include <QPair>
#include <KConfigGroup>
class MetaListViewWidget;
class QPoint;
class QItemSelection;
class QAction;
class QAbstractItemModel;
class ProxyModel;
class QSqlTableModel;
class QModelIndex;

class MetaListViewPart : public KParts::Part
{
	Q_OBJECT
public:
	MetaListViewPart(QObject *parent = 0);

protected:
	QModelIndex selectedIndex() const;
	QSqlTableModel *model() const;
	void setModel(QSqlTableModel *model, int displayColumn,
		      const KConfigGroup & metaListViewSettings = KConfigGroup());
	void addSelectionDependentAction(const QString & actionName);
	void addMetabarFieldMapping(const QString & label, const QString & field);

protected slots:
	virtual void loadModel() = 0;
	virtual void itemActivated(const QModelIndex & index);
	//virtual void itemSelected(const QModelIndex & index);

private slots:
	void slotItemActivated(const QModelIndex & proxyIndex);
	void slotSelectionChanged(const QItemSelection & newSelection);
	void slotContextMenuRequested(const QModelIndex & index);

	void updateSelectionDependentActions();

	//these are convenience slots that provide setMetaBarPosition() and setListViewMode()
	//with int parameters so that they are able to connect directly to QAction::triggered()
	void changeViewMode(int view_mode_index);
	void changeMetaBarPosition(int metabar_position_index);

private:
	MetaListViewWidget *m_widget;
	ProxyModel *m_proxyModel;
	QList< QPair<QString, QString> > m_fieldMapList;
};

#endif
