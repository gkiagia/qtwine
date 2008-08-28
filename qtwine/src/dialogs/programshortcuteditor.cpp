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
#include "programshortcuteditor.h"
#include "../utils/urlrequestercapabledelegate.h"
#include "../widgets/iconrequesterbutton.h"
#include "../widgets/winedlloverridesrequester.h"
#include "../qtwineapplication.h"
#include "qtwinepreferences.h"

#include <KLineEdit>
#include <KUrlRequester>
#include <KLocalizedString>
#include <KDebug>

#include <QLabel>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QFrame>
#include <QDataWidgetMapper>

ProgramShortcutEditor::ProgramShortcutEditor(const QModelIndex & index, QWidget *parent)
    : EditorPageDialog(parent)
{
    Q_ASSERT(index.isValid());

    const QSqlTableModel *model = qobject_cast<const QSqlTableModel*>(index.model());
    Q_ASSERT(model);
    QSqlRelationalDelegate *delegate = new UrlRequesterCapableDelegate(this);

    /* setup gui */
    setCaption(makeStandardCaption(i18n("Program Shortcut Editor"), this));

    QWidget *page = new QWidget(this);
    KPageWidgetItem *pageItem = addPage(page, i18n("Shortcut properties"));
    pageItem->setIcon(KIcon("document-properties"));

    QLabel *nameLabel = new QLabel(i18n("Name:"), page);
    KLineEdit *nameEdit = new KLineEdit(page);
    QVBoxLayout *nameLayout = new QVBoxLayout;
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);

    IconRequesterButton *iconButton = new IconRequesterButton(page);
    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(iconButton);
    iconLayout->addLayout(nameLayout);

    QFrame *hLine = new QFrame(page);
    hLine->setFrameShape(QFrame::HLine);

    QFormLayout *formLayout = new QFormLayout;
    executableEdit = new KUrlRequester(page);
    executableEdit->setMode(KFile::File | KFile::LocalOnly | KFile::ExistingOnly);
    connect(executableEdit, SIGNAL(urlSelected(KUrl)), this, SLOT(slotExecutableChanged(KUrl)) );
    formLayout->addRow(i18n("&Executable:"), executableEdit);

    KLineEdit *argumentsEdit = new KLineEdit(page);
    formLayout->addRow(i18n("&Arguments:"), argumentsEdit);

    workdirEdit = new KUrlRequester(page);
    workdirEdit->setMode(KFile::Directory | KFile::LocalOnly | KFile::ExistingOnly);
    formLayout->addRow(i18n("&Working directory:"), workdirEdit);

    QModelIndex configurationColumnIndex =
                model->index(index.row(), model->fieldIndex("wine_configurations_name"));
    QWidget *configurationEdit =
                delegate->createEditor(this, QStyleOptionViewItem(), configurationColumnIndex);
    formLayout->addRow(i18n("Uses wine &configuration:"), configurationEdit);
    
    WineDllOverridesRequester *dllOverridesEdit = new WineDllOverridesRequester(this);
    formLayout->addRow(i18n("Wine &dll overrides:"), dllOverridesEdit);

    QGroupBox *optionsGroup = new QGroupBox(i18n("Options"), page);
    QCheckBox *terminalBox = new QCheckBox(i18n("Show debugging output in a terminal"), page);
    QCheckBox *cuiAppBox = new QCheckBox(i18n("This is a Windows CUI (DOS) application"), page);
    QVBoxLayout *optionsGroupLayout = new QVBoxLayout(optionsGroup);
    optionsGroupLayout->addWidget(terminalBox);
    optionsGroupLayout->addWidget(cuiAppBox);

    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->addLayout(iconLayout);
    mainLayout->addWidget(hLine);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(optionsGroup);

    /* map data */
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(const_cast<QSqlTableModel*>(model));
    mapper->setItemDelegate(delegate);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    mapper->addMapping(iconButton, model->fieldIndex("icon"));
    mapper->addMapping(nameEdit, model->fieldIndex("name"));
    mapper->addMapping(executableEdit, model->fieldIndex("executable"));
    mapper->addMapping(argumentsEdit, model->fieldIndex("arguments"));
    mapper->addMapping(workdirEdit, model->fieldIndex("workdir"));
    mapper->addMapping(configurationEdit, model->fieldIndex("wine_configurations_name"));
    mapper->addMapping(terminalBox, model->fieldIndex("run_in_terminal"));
    mapper->addMapping(cuiAppBox, model->fieldIndex("is_cui_application"));
    mapper->addMapping(dllOverridesEdit, model->fieldIndex("winedlloverrides"));
    mapper->setCurrentModelIndex(index);

    connect(iconButton, SIGNAL(iconChanged()), SLOT(enableChangesDependentButtons()) );
    connect(nameEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
    connect(executableEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
    connect(argumentsEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
    connect(workdirEdit, SIGNAL(textChanged(QString)), SLOT(enableChangesDependentButtons()) );
    connect(configurationEdit, SIGNAL(currentIndexChanged(int)), SLOT(enableChangesDependentButtons()) );
    connect(terminalBox, SIGNAL(toggled(bool)), SLOT(enableChangesDependentButtons()) );
    connect(cuiAppBox, SIGNAL(toggled(bool)), SLOT(enableChangesDependentButtons()) );
    connect(dllOverridesEdit, SIGNAL(overridesChanged()), SLOT(enableChangesDependentButtons()) );
    
    /* This is a HACK to load the default configuration in the combo box
     * because setting it from ShortcutsModel::slotPrimeInsert doesn't work
     * for some strange reason...
     */
    QComboBox *configurationCombo = qobject_cast<QComboBox*>(configurationEdit);
    Q_ASSERT(configurationCombo);
    if ( configurationCombo->currentIndex() < 0 ) {
        kDebug() << "Configuration not set. using the default:" << QtWinePreferences::defaultWineConfiguration();
        QtWineSqlTableModel *cfgModel = qtwineApp->wineConfigurationsModel();
        configurationCombo->setCurrentIndex(cfgModel->idToRow(QtWinePreferences::defaultWineConfiguration()));
    }
}

void ProgramShortcutEditor::slotExecutableChanged(const KUrl & newUrl)
{
    workdirEdit->setPath(newUrl.directory());
}

bool ProgramShortcutEditor::applyChanges()
{
    return mapper->submit();
}

bool ProgramShortcutEditor::revertChanges()
{
    mapper->revert();
    return true;
}

#include "programshortcuteditor.moc"
