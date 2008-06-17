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
#include "installationeditor.h"
#include "../utils/urlrequestercapabledelegate.h"

#include <QModelIndex>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QFormLayout>
#include <QItemDelegate>

#include <KMessageBox>
#include <KLocalizedString>
#include <KLineEdit>
#include <KUrlRequester>

InstallationEditor::InstallationEditor(const QModelIndex & index, QWidget *parent)
	: EditorPageDialog(parent)
{
	Q_ASSERT(index.isValid());

	/* setup gui */
	setCaption(makeStandardCaption(i18n("Wine Installation Editor"), this));

	QWidget *w = new QWidget(this);
	KPageWidgetItem *propertiesPageItem = addPage(w, i18n("Properties"));
	propertiesPageItem->setIcon(KIcon("document-properties"));

	QFormLayout *formLayout = new QFormLayout(w);
	resizeLayout(formLayout, marginHint(), spacingHint());

	KLineEdit *nameEdit = new KLineEdit(w);
	formLayout->addRow(i18n("&Name:"), nameEdit);

	KUrlRequester *prefixEdit = new KUrlRequester(w);
	prefixEdit->setMode(KFile::Directory | KFile::LocalOnly | KFile::ExistingOnly);
	formLayout->addRow(i18n("&Prefix directory:"), prefixEdit);

	KUrlRequester *wineLoaderEdit = new KUrlRequester(w);
	wineLoaderEdit->setMode(KFile::File | KFile::LocalOnly | KFile::ExistingOnly);
	formLayout->addRow(i18n("Path to &wine executable ($WINELOADER):"), wineLoaderEdit);

	KUrlRequester *wineServerEdit = new KUrlRequester(w);
	wineServerEdit->setMode(KFile::File | KFile::LocalOnly | KFile::ExistingOnly);
	formLayout->addRow(i18n("Path to wine&server executable ($WINESERVER):"), wineServerEdit);

	KUrlRequester *wineDllPathEdit = new KUrlRequester(w);
	wineDllPathEdit->setMode(KFile::Directory | KFile::LocalOnly | KFile::ExistingOnly);
	formLayout->addRow(i18n("Path to wine &dll directory ($WINEDLLPATH):"), wineDllPathEdit);

	/* map data */
	mapper = new QDataWidgetMapper(this);
	mapper->setModel(const_cast<QAbstractItemModel*>(index.model()));
	mapper->setItemDelegate(new UrlRequesterCapableDelegate(mapper));
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

	const QSqlTableModel *model = qobject_cast<const QSqlTableModel*>(index.model());
	Q_ASSERT(model);

	mapper->addMapping(nameEdit, model->fieldIndex("name"));
	mapper->addMapping(prefixEdit, model->fieldIndex("prefix"));
	mapper->addMapping(wineLoaderEdit, model->fieldIndex("wineloader"));
	mapper->addMapping(wineServerEdit, model->fieldIndex("wineserver"));
	mapper->addMapping(wineDllPathEdit, model->fieldIndex("winedllpath"));
	mapper->setCurrentModelIndex(index);

	connect(nameEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
	connect(prefixEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
	connect(wineLoaderEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
	connect(wineServerEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
	connect(wineDllPathEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
}

bool InstallationEditor::applyChanges()
{
#if 0
	QString wineloader = dlg.wineLoaderEdit->url().path();

	if ( !QFile::exists(wineloader) or !QFileInfo(wineloader).isFile() ) {
		KMessageBox::ButtonCode answer =
			KMessageBox::warningYesNo(this, i18n("QtWine could not find %1\n"
			" It seems that in the prefix directory that you have selected there is no"
			" wine installation. Are you sure you want to continue ?").arg(wineloader));
		if (answer == KMessageBox::No)
			return;
	}
#endif
	return mapper->submit();
}

bool InstallationEditor::revertChanges()
{
	mapper->revert();
	return true;
}

#include "installationeditor.moc"
