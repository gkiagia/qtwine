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
#include "metalistviewwidget.h"
#include "metabar.h"
#include "../qtwineapplication.h"
#include "qtwinepreferences.h"
#include <QSplitter>
#include <QListView>
#include <QList>
#include <KGlobalSettings>
#include <KDebug>
#include <QMetaEnum>
#include <QVBoxLayout>

#define METABAR_SIZE 200
#define HOVER_SELECT_DELAY 200

KCONFIGGROUP_DECLARE_ENUM_QOBJECT(MetaListViewWidget, MetaBarPosition)
KCONFIGGROUP_DECLARE_ENUM_QOBJECT(QListView, ViewMode)


MetaListViewWidget::MetaListViewWidget(QWidget *parent)
	: QWidget(parent),
	  m_activationClickMode(DoubleClick) //this is the default mode of QListView
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	m_splitter = new QSplitter(this);
	layout->addWidget(m_splitter);

	m_metaBar = new MetaBar;
	m_listView = new QListView;

	m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
	m_listView->setIconSize(QSize(32, 32));
	setMetaBarPosition(Right); // This puts the widgets in the splitter.

	m_hoverSelectTimer.setSingleShot(true);
	connect( &m_hoverSelectTimer, SIGNAL(timeout()), SLOT(slotHoverSelectTimerTimeout()) );
}

MetaListViewWidget::~MetaListViewWidget()
{
    if ( m_configGroup.isValid() )
        saveSettings(m_configGroup);
}

void MetaListViewWidget::initialize(QAbstractItemModel *model, const KConfigGroup & group)
{
	m_listView->setModel(model);

	if ( group.isValid() ) {
		setMetaBarPosition( readEntry(group, "metaBarPosition", Right) );
		setListViewMode( readEntry(group, "listViewMode", QListView::ListMode) );
                m_configGroup = group;
	} else {
		setMetaBarPosition(Right);
		setListViewMode(QListView::ListMode);
	}

	setMultipleSelectionEnabled(false); // this should not be user-configurable. depends on the model.

	QItemSelectionModel *selModel = m_listView->selectionModel();
	connect(selModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection )),
		SIGNAL(selectionChanged(QItemSelection)) );

	connect(m_listView, SIGNAL(customContextMenuRequested(QPoint)),
		SLOT(slotContextMenuRequested(QPoint)) );

    loadGlobalPreferences();
    connect(qtwineApp, SIGNAL(preferencesChanged()), this, SLOT(loadGlobalPreferences()) );
}

void MetaListViewWidget::loadGlobalPreferences()
{
    setActivationClickMode( static_cast<ActivationClickMode>(QtWinePreferences::clickMode()) );
}

void MetaListViewWidget::saveSettings(KConfigGroup & group)
{
	writeEntry(group, "metaBarPosition", metaBarPosition());
	writeEntry(group, "listViewMode", listViewMode());
}


MetaListViewWidget::MetaBarPosition MetaListViewWidget::metaBarPosition() const
{
	return m_metaBarPosition;
}

void MetaListViewWidget::setMetaBarPosition(MetaBarPosition pos)
{
	m_metaBarPosition = pos;

	m_splitter->insertWidget(static_cast<int>(m_metaBarPosition != Left), m_metaBar);
	m_splitter->insertWidget(static_cast<int>(m_metaBarPosition == Left), m_listView);

	QList<int> splitterSizes;
	if ( m_metaBarPosition == Hidden ) {
		m_metaBar->hide();
		splitterSizes << m_splitter->width() << 0; // metabar should be on the right side.
	} else {
		m_metaBar->show();
		splitterSizes << ((m_metaBarPosition == Left) ?
				METABAR_SIZE : (m_splitter->width() - METABAR_SIZE));
		splitterSizes << ((m_metaBarPosition == Right) ?
				METABAR_SIZE : (m_splitter->width() - METABAR_SIZE));
	}
	m_splitter->setSizes(splitterSizes);
}

QListView::ViewMode MetaListViewWidget::listViewMode() const
{
	return m_listView->viewMode();
}

void MetaListViewWidget::setListViewMode(QListView::ViewMode m)
{
	if ( m_listView->viewMode() == m ) return;

	m_listView->setViewMode(m);

	if ( m == QListView::IconMode ) {
		m_listView->setGridSize(QSize(90, 90));
		m_listView->setWordWrap(true);
		m_listView->setFlow(QListView::LeftToRight);
		m_listView->setResizeMode(QListView::Adjust);
		m_listView->setWrapping(true);
	} else {
		m_listView->setGridSize(QSize());
		m_listView->setWordWrap(false);
		m_listView->setFlow(QListView::TopToBottom);
		m_listView->setResizeMode(QListView::Fixed);
		m_listView->setWrapping(false);
	}
}

bool MetaListViewWidget::multipleSelectionEnabled() const
{
	return m_listView->selectionMode() == QAbstractItemView::ExtendedSelection;
}

void MetaListViewWidget::setMultipleSelectionEnabled(bool enabled)
{
	if ( enabled )
		m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	else
		m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
}

MetaListViewWidget::ActivationClickMode MetaListViewWidget::activationClickMode() const
{
	return m_activationClickMode;
}

void MetaListViewWidget::setActivationClickMode(ActivationClickMode mode)
{
	bool singleClickEnabled = false;
	switch (mode) {
		case UseKdeDefault:
			singleClickEnabled = KGlobalSettings::singleClick();
			break;
		case SingleClick:
			singleClickEnabled = true;
			break;
		case DoubleClick:
			singleClickEnabled = false;
			break;
	}

	m_activationClickMode = mode;
	disconnect(m_listView, 0, this, SIGNAL(itemActivated(QModelIndex)) );

	if ( singleClickEnabled ) {
		//FIXME clicked() is emited even when we are selecting multiple items with ctrl+click
		connect(m_listView, SIGNAL(clicked(QModelIndex)), SIGNAL(itemActivated(QModelIndex)) );
		m_listView->setMouseTracking(true);
		connect(m_listView, SIGNAL(entered(QModelIndex)), SLOT(slotItemHovered(QModelIndex)) );
	} else {
		connect(m_listView, SIGNAL(doubleClicked(QModelIndex)), SIGNAL(itemActivated(QModelIndex)) );
		m_listView->setMouseTracking(false);
		disconnect(m_listView, SIGNAL(entered(QModelIndex)),
			   this, SLOT(slotItemHovered(QModelIndex)) );
	}
}

void MetaListViewWidget::slotItemHovered(const QModelIndex & index)
{
	int delay = KGlobalSettings::autoSelectDelay();
	if ( delay == -1 ) {
		if ( multipleSelectionEnabled() )
			return; // because it is possible to select items using ctrl or dragging
		else
			delay = HOVER_SELECT_DELAY;
	}

	if ( m_listView->selectionModel()->selectedIndexes().size() <= 1 ) {
		m_hoveredIndex = index;
		m_hoverSelectTimer.stop();
		m_hoverSelectTimer.start(delay);
	}
}

/*! selects m_hoveredIndex in the listView */
void MetaListViewWidget::slotHoverSelectTimerTimeout()
{
	m_listView->selectionModel()->select(m_hoveredIndex,
				QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
}

/*! translates \a pos to a model index and emits contextMenuRequested() */
void MetaListViewWidget::slotContextMenuRequested(const QPoint & pos)
{
	QModelIndex index = m_listView->indexAt(pos);
	emit contextMenuRequested(index);
}

#include "metalistviewwidget.moc"
