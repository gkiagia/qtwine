/***************************************************************************
 *   Copyright (C) 2007-2008 by George Kiagiadakis                         *
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
#include "metalistviewpart.h"
#include "../widgets/metalistviewwidget.h"
#include "../widgets/metabar.h"

#include <QMenu>
#include <QSortFilterProxyModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QItemSelection>
#include <QCursor>

#include <KXMLGUIClient>
#include <KXMLGUIFactory>
#include <KSelectAction>
#include <KIcon>
#include <KActionCollection>
#include <KLocalizedString>


class MetaListViewPart::ProxyModel : public QSortFilterProxyModel
{
public:
    ProxyModel(MetaListViewPart *parent) : QSortFilterProxyModel(parent), m_metaListViewPart(parent) {}
    QVariant data (const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
    
private:
    friend class MetaListViewPart;
    MetaListViewPart *m_metaListViewPart;
};

/*! Reimplemented to export the Qt::DecorationRole. */
QVariant MetaListViewPart::ProxyModel::data(const QModelIndex & index, int role) const
{
    if ( index.isValid() and (role == Qt::DecorationRole or role == Qt::DisplayRole) ) {
        QSqlQueryModel *m = qobject_cast<QSqlQueryModel*>(sourceModel());
        Q_ASSERT(m);
        QModelIndex sourceIndex = mapToSource(index);
        Q_ASSERT(sourceIndex.isValid());
        
        switch(role) {
            case Qt::DecorationRole:
            {
                QString icon = m->record(sourceIndex.row()).value("icon").toString();
                return !icon.isEmpty() ? QIcon(icon) : KIcon("wine");
                break;
            }
            case Qt::DisplayRole:
            {
                return ( sourceIndex.row() == m_metaListViewPart->m_defaultRow )
                            ? sourceIndex.data(role).toString().prepend("* ") : sourceIndex.data(role);
                break;
            }
            default:
                Q_ASSERT(false);
        }
    }

    return QSortFilterProxyModel::data(index, role);
}

/*! Reimplemented to disable editing by the delegate. */
Qt::ItemFlags MetaListViewPart::ProxyModel::flags(const QModelIndex & index) const
{
    return QSortFilterProxyModel::flags(index) & ~(Qt::ItemIsEditable);
}

// PUBLIC

MetaListViewPart::MetaListViewPart(QObject *parent)
    : KParts::Part(parent), m_proxyModel(NULL), m_defaultRow(-1)
{
    m_widget = new MetaListViewWidget;
    setWidget(m_widget);

    connect(m_widget, SIGNAL(itemActivated(QModelIndex)),
            SLOT(slotItemActivated(QModelIndex)) );
    connect(m_widget, SIGNAL(selectionChanged(QItemSelection)),
            SLOT(slotSelectionChanged(QItemSelection)) );
    connect(m_widget, SIGNAL(contextMenuRequested(QModelIndex)),
            SLOT(slotContextMenuRequested(QModelIndex)) );

    // Show initial information on the metaBar
    slotSelectionChanged(QItemSelection());

    //populate the listView (later, in order to respect the size of the listView)
    QMetaObject::invokeMethod(this, "loadModel", Qt::QueuedConnection);

    /* ACTIONS */
    KSelectAction *view_mode = new KSelectAction(i18n("View mode"), this);
    connect(view_mode, SIGNAL(triggered(int)), SLOT(changeViewMode(int)) );
    actionCollection()->addAction("view_mode", view_mode);
    actionCollection()->addAction("view_mode_list",
                view_mode->addAction(KIcon("view-list-details"), i18n("List")) );
    actionCollection()->addAction("view_mode_icons",
                view_mode->addAction(KIcon("view-list-icons"), i18n("Icons")) );

    KSelectAction *metabar_position = new KSelectAction(i18n("Metabar position"), this);
    connect(metabar_position, SIGNAL(triggered(int)), SLOT(changeMetaBarPosition(int)) );
    actionCollection()->addAction("metabar_position", metabar_position);
    actionCollection()->addAction("metabar_position_left",
                metabar_position->addAction(KIcon("view-list-details"), i18n("Left")) );
    actionCollection()->addAction("metabar_position_right",
                metabar_position->addAction(KIcon("view-list-icons"), i18n("Right")) );
    actionCollection()->addAction("metabar_position_hidden",
                metabar_position->addAction(KIcon("view-list-icons"), i18n("Hidden")) );
}

// PROTECTED

QModelIndex MetaListViewPart::selectedIndex() const
{
    if ( !m_widget->listView()->selectionModel() ) //this is possible if no model is set.
        return QModelIndex();

    QModelIndexList indexes = m_widget->listView()->selectionModel()->selectedIndexes();
    if ( indexes.size() == 1 )
        return m_proxyModel->mapToSource(indexes.at(0));
    else
        return QModelIndex();
}

QSqlTableModel *MetaListViewPart::model() const
{
    return m_proxyModel ? static_cast<QSqlTableModel*>(m_proxyModel->sourceModel()) : NULL;
}

void MetaListViewPart::setModel(QSqlTableModel *model, int displayColumn,
                                const KConfigGroup & metaListViewSettings)
{
    delete m_proxyModel; // delete the previous proxy model

    m_proxyModel = new ProxyModel(this);
    m_proxyModel->setSourceModel(model);
    m_widget->initialize(m_proxyModel, metaListViewSettings);
    m_widget->listView()->setModelColumn(displayColumn);
    updateSelectionDependentActions();

    //update MetaListViewWidget-properties-related actions
    static_cast<KSelectAction*>(actionCollection()->action("view_mode"))
                    ->setCurrentItem( static_cast<int>(m_widget->listViewMode()) );
    static_cast<KSelectAction*>(actionCollection()->action("metabar_position"))
                    ->setCurrentItem( static_cast<int>(m_widget->metaBarPosition()) );

}

void MetaListViewPart::addMetabarFieldMapping(const QString & label, const QString & field)
{
    m_fieldMapList << qMakePair(label, field);
}

void MetaListViewPart::addSelectionDependentAction(const QString & actionName)
{
    addStateActionDisabled("deselected_item_state", actionName);
}

void MetaListViewPart::addDefaultItemDependentAction(const QString & actionName, bool enabledOnDefaultItem)
{
    if ( enabledOnDefaultItem == false )
        addStateActionDisabled("default_item_selected_state", actionName);
    else
        addStateActionEnabled("default_item_selected_state", actionName);
}

void MetaListViewPart::enableDefaultItem(const QString & setDefaultAction, int currentDefaultRow)
{
    addDefaultItemDependentAction(setDefaultAction, false); /*disabled on the default item*/
    addSelectionDependentAction(setDefaultAction);
    connect(action(setDefaultAction.toAscii()), SIGNAL(triggered()), SLOT(setCurrentItemAsDefault()) );
    updateDefaultItemRow(currentDefaultRow);
}

void MetaListViewPart::itemActivated(const QModelIndex &) {}

void MetaListViewPart::updateDefaultItemRow(int newDefaultRow)
{
    int oldDefaultRow = m_defaultRow;
    m_defaultRow = newDefaultRow;
    emit defaultItemRowUpdated(newDefaultRow, oldDefaultRow);
    
    Q_ASSERT(m_proxyModel);

    QModelIndex oldDefaultIndex = m_proxyModel->mapFromSource(
                                    model()->index(oldDefaultRow, m_widget->listView()->modelColumn()) );
    QModelIndex newDefaultIndex = m_proxyModel->mapFromSource(
                                    model()->index(newDefaultRow, m_widget->listView()->modelColumn()) );
    emit m_proxyModel->dataChanged(oldDefaultIndex, oldDefaultIndex);
    emit m_proxyModel->dataChanged(newDefaultIndex, newDefaultIndex);
    
    //update the actions because if the new default action is selected
    //when this function is called, the "default_item_selected_state" must change.
    updateSelectionDependentActions();
}

// PRIVATE

/*! called from the "setDefault" action */
void MetaListViewPart::setCurrentItemAsDefault()
{
    QModelIndex selectedItem = selectedIndex();
    Q_ASSERT(selectedItem.isValid());
    updateDefaultItemRow(selectedItem.row());
}

/*! enables or disables selection dependent actions */
void MetaListViewPart::updateSelectionDependentActions()
{
    QModelIndex selectedItem = selectedIndex();
    bool anItemIsSelected = selectedItem.isValid();
    stateChanged("deselected_item_state",
                 static_cast<KXMLGUIClient::ReverseStateChange>(anItemIsSelected));
    if (anItemIsSelected) {
        stateChanged("default_item_selected_state",
                     static_cast<KXMLGUIClient::ReverseStateChange>(!(selectedItem.row() == m_defaultRow)));
    }
}

/*! Calls itemActivated(const QModelIndex&) with an index referring to the source model. */
void MetaListViewPart::slotItemActivated(const QModelIndex & proxyIndex)
{
    Q_ASSERT(m_proxyModel);
    itemActivated(m_proxyModel->mapToSource(proxyIndex));
}

void MetaListViewPart::slotSelectionChanged(const QItemSelection & newSelection)
{
    MetaBar *metaBar = m_widget->metaBar();
    metaBar->clearInformation();

    if ( newSelection.size() != 1 ) {
        metaBar->addInformationPair( i18n("Items selected"),
                                     QString::number(newSelection.size()) );
        metaBar->setIcon(KIcon("dialog-information"));
    } else {

        /* convert the index to an index of the source model */
        QModelIndex proxyIndex = newSelection.indexes().at(0);
        Q_ASSERT(m_proxyModel);
        QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);

        const QSqlTableModel *model = qobject_cast<const QSqlTableModel*>(sourceIndex.model());
        Q_ASSERT(model);
        QSqlRecord record = model->record(sourceIndex.row());

        QPair<QString, QString> fieldMapping;
        foreach(fieldMapping, m_fieldMapList) {
            metaBar->addInformationPair( fieldMapping.first,
                                record.value(fieldMapping.second).toString() );
        }

        //get the icon from ProxyModel::data()
        metaBar->setIcon(proxyIndex.data(Qt::DecorationRole).value<QIcon>());
    }

    updateSelectionDependentActions();
}

void MetaListViewPart::slotContextMenuRequested(const QModelIndex & index)
{
    QString menu_name;
    if ( index.isValid() )
        menu_name = "listView_context_menu_selected";
    else
        menu_name = "listView_context_menu_deselected";

    QMenu *popup = static_cast<QMenu*>(factory()->container(menu_name,this));
    if ( popup ) popup->exec(QCursor::pos());
}


void MetaListViewPart::changeViewMode(int view_mode_index)
{
    m_widget->setListViewMode(static_cast<QListView::ViewMode>(view_mode_index));
}

void MetaListViewPart::changeMetaBarPosition(int metabar_position_index)
{
    m_widget->setMetaBarPosition(static_cast<MetaListViewWidget::MetaBarPosition>(metabar_position_index));
}

#include "metalistviewpart.moc"
