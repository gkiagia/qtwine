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
#include "configurationeditor.h"

#include <QSqlRelationalDelegate>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QFormLayout>

#include <KIcon>
#include <KLocalizedString>
#include <KLineEdit>
#include <KTextEdit>

ConfigurationEditor::ConfigurationEditor(const QModelIndex & index, QWidget *parent)
	: EditorPageDialog(parent)
{
	Q_ASSERT(index.isValid());

	const QSqlTableModel *model = qobject_cast<const QSqlTableModel*>(index.model());
	Q_ASSERT(model);

	/* setup gui */
	setCaption(makeStandardCaption(i18n("Wine Configuration Editor"), this));

	QWidget *propertiesPage = new QWidget(this);
	KPageWidgetItem *propertiesPageItem = addPage(propertiesPage, i18n("Properties"));
	propertiesPageItem->setIcon(KIcon("document-properties"));

	QFormLayout *formLayout = new QFormLayout(propertiesPage);
	resizeLayout(formLayout, marginHint(), spacingHint());

	KLineEdit *nameEdit = new KLineEdit(propertiesPage);
	formLayout->addRow(i18n("&Name:"), nameEdit);

	QSqlRelationalDelegate *delegate = new QSqlRelationalDelegate(this);
	QModelIndex installationColumnIndex =
		index.model()->index(index.row(), model->fieldIndex("wine_installations_name"));
	QWidget *installationEdit =
		delegate->createEditor(this, QStyleOptionViewItem(), installationColumnIndex);
	formLayout->addRow(i18n("Uses &installation:"), installationEdit);

	KTextEdit *descriptionTextEdit = new KTextEdit(propertiesPage);
	descriptionTextEdit->setAcceptRichText(false);
	formLayout->addRow(i18n("&Description / Notes:"), descriptionTextEdit);

    resizeLayout(formLayout, marginHint(), spacingHint());

	/* map data */
	mapper = new QDataWidgetMapper(this);
	mapper->setModel(const_cast<QAbstractItemModel*>(index.model()));
	mapper->setItemDelegate(delegate);
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

	mapper->addMapping(nameEdit, model->fieldIndex("name"));
	mapper->addMapping(installationEdit, model->fieldIndex("wine_installations_name"));
	mapper->addMapping(descriptionTextEdit, model->fieldIndex("description"), "plainText");
	mapper->setCurrentModelIndex(index);

	connect(nameEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
	connect(installationEdit, SIGNAL(currentIndexChanged(int)), SLOT(enableChangesDependentButtons()) );
	connect(descriptionTextEdit, SIGNAL(textChanged()), SLOT(enableChangesDependentButtons()) );
}

bool ConfigurationEditor::applyChanges()
{
	return mapper->submit();
}

bool ConfigurationEditor::revertChanges()
{
	mapper->revert();
	return true;
}

#include "configurationeditor.moc"
