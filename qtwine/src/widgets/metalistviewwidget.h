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
#ifndef METALISTVIEWWIDGET_H
#define METALISTVIEWWIDGET_H

#include <QWidget>
#include <QListView>
#include <QTimer>
#include <QMetaType>
#include <KConfigGroup>
class QSplitter;
class MetaBar;

class MetaListViewWidget : public QWidget
{
	Q_OBJECT
	Q_ENUMS(MetaBarPosition)
	Q_ENUMS(ActivationClickMode)
	Q_PROPERTY(MetaBarPosition metaBarPosition READ metaBarPosition WRITE setMetaBarPosition)
	Q_PROPERTY(QListView::ViewMode listViewMode READ listViewMode WRITE setListViewMode)
	Q_PROPERTY(bool multipleSelectionEnabled READ multipleSelectionEnabled
			WRITE setMultipleSelectionEnabled)
	Q_PROPERTY(ActivationClickMode activationClickMode READ activationClickMode
			WRITE setActivationClickMode)
public:
	enum MetaBarPosition { Left, Right, Hidden };
	enum ActivationClickMode { UseKdeDefault, SingleClick, DoubleClick };

	MetaListViewWidget(QWidget *parent = 0);

	QListView *listView() const { return m_listView; }
	MetaBar *metaBar() const { return m_metaBar; }

	MetaBarPosition metaBarPosition() const;
	void setMetaBarPosition(MetaBarPosition pos);

	QListView::ViewMode listViewMode() const;
	void setListViewMode(QListView::ViewMode m);

	bool multipleSelectionEnabled() const;
	void setMultipleSelectionEnabled(bool enabled);

	ActivationClickMode activationClickMode() const;
	void setActivationClickMode(ActivationClickMode mode);

public slots:
	void initialize(QAbstractItemModel *model, const KConfigGroup & group = KConfigGroup());
	void saveSettings(KConfigGroup & group);

signals:
	void itemActivated(const QModelIndex & index);
	void selectionChanged(const QItemSelection & selected);
	void contextMenuRequested(const QModelIndex & index);

private slots:
	void slotItemHovered(const QModelIndex & index);
	void slotHoverSelectTimerTimeout();
	void slotContextMenuRequested(const QPoint & pos);

private:
	QListView *m_listView;
	QSplitter *m_splitter;
	MetaBar *m_metaBar;
	MetaBarPosition m_metaBarPosition;
	ActivationClickMode m_activationClickMode;

	QModelIndex m_hoveredIndex;
	QTimer m_hoverSelectTimer;
};

#endif
