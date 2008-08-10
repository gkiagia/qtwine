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
class QSqlTableModel;
class QModelIndex;

class MetaListViewPart : public KParts::Part
{
    Q_OBJECT
public:
    MetaListViewPart(QObject *parent = 0);

protected:
    /*! Returns the currently selected index */
    QModelIndex selectedIndex() const;
    
    /*! Returns a pointer to the model that has been set */
    QSqlTableModel *model() const;
    
    /*! Sets the specified \a model to appear in the listview.
     * The column that appears will be \a displayColumn.
     * \a metaListViewSettings is passed to MetaListViewWidget::initialize()
     * to load settings for this view. It can be ommited to load the defaults.
     */
    void setModel(QSqlTableModel *model, int displayColumn,
                  const KConfigGroup & metaListViewSettings = KConfigGroup());
    
    /*! Maps the specified sql \a field to appear in metabar with the
     * specified \a label and the value of the selected item */
    void addMetabarFieldMapping(const QString & label, const QString & field);
    
    /*! Marks the specified action to be enabled only if an item is selected */
    void addSelectionDependentAction(const QString & actionName);
    
    /*! Marks the specified action to depend on whether the default item is selected or not.
     * The action will be enabled for the default item if \a enabledOnDefaultItem is true,
     * or enabled on all other items except the default if \a enabledOnDefaultItem is false.
     */
    void addDefaultItemDependentAction(const QString & actionName, bool enabledOnDefaultItem);
    
    /*! Enables the concept of having one "default" item in the list. The current default item's 
     * row is indicated by \a currentDefaultRow and the action that should change the
     * default item is indicated by \a setDefaultAction.
     * \warning do not call if a model is not set yet.
     */
    void enableDefaultItem(const QString & setDefaultAction, int currentDefaultRow);
    
signals:
    void defaultItemRowUpdated(int newDefaultRow, int oldDefaultRow);

protected slots:
    /*! This slot must be implemented to load the model.
     * The implementation must use setModel() to set the model.
     */
    virtual void loadModel() = 0;
    
    /*! This is called when an item is activated
     * (with single or double click, depending on settings).
     */
    virtual void itemActivated(const QModelIndex & index);
    
    /*! makes the default item to be the item at the \a newDefaultRow row */
    void updateDefaultItemRow(int newDefaultRow);

private slots:
    void slotItemActivated(const QModelIndex & proxyIndex);
    void slotSelectionChanged(const QItemSelection & newSelection);
    void slotContextMenuRequested(const QModelIndex & index);
    void setCurrentItemAsDefault();
    void updateSelectionDependentActions();

    //these are convenience slots that provide setMetaBarPosition() and setListViewMode() (of m_widget)
    //with int parameters so that they are able to connect directly to QAction::triggered()
    void changeViewMode(int view_mode_index);
    void changeMetaBarPosition(int metabar_position_index);

private:
    class ProxyModel;

    MetaListViewWidget *m_widget;
    ProxyModel *m_proxyModel;
    QList< QPair<QString, QString> > m_fieldMapList;
    int m_defaultRow;
};

#endif
