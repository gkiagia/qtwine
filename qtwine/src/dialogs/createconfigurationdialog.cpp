/*
    Copyright (C) 2008  George Kiagiadakis <gkiagia@users.sourceforge.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "createconfigurationdialog.h"
#include "ui_createConfigurationDialog.h"
#include "qtwinepreferences.h"
#include "../qtwineapplication.h"
#include "../providers/wineconfigurationsprovider.h"
#include "../providers/wineinstallationsprovider.h"

#include <KLocalizedString>
#include <KMessageBox>

//this is the main widget of the dialog, loaded from the ui file.
class MainWidget : public QWidget, public Ui::CreateConfigurationDialog
{
public:
    MainWidget(QWidget *parent = 0) : QWidget(parent) {
        setupUi(this);
    }
};

CreateConfigurationDialog::CreateConfigurationDialog(QWidget *parent)
    : KDialog(parent)
{
    MainWidget *m = new MainWidget(this);
    setMainWidget(m);
    setCaption( i18n("Create a new wine configuration") );
    setButtons( KDialog::Ok | KDialog::Cancel );
    setCreationMode(true); // default is "create" mode
    connect(m->createRadio, SIGNAL(toggled(bool)), SLOT(setCreationMode(bool)) );
}

/* true = create, false = copy. bool argument is to connect to QAbstractButton::toggled(bool) */
void CreateConfigurationDialog::setCreationMode(bool create)
{
    MainWidget *m = static_cast<MainWidget*>(mainWidget());
    if (create) {
        m->comboBox->setModel(installationsProvider->model());
        m->comboBox->setModelColumn(installationsProvider->model()->fieldIndex("name"));
        m->comboBox->setCurrentIndex(installationsProvider->idToRow(QtWinePreferences::defaultWineInstallation()));
        m->comboLabel->setText(i18n("Wine installation to use:"));
    } else {
        m->comboBox->setModel(configurationsProvider->model());
        m->comboBox->setModelColumn(configurationsProvider->model()->fieldIndex("name"));
        m->comboBox->setCurrentIndex(configurationsProvider->idToRow(QtWinePreferences::defaultWineConfiguration()));
        m->comboLabel->setText(i18n("Wine configuration to copy:"));
    }
}

void CreateConfigurationDialog::accept()
{
    MainWidget *m = static_cast<MainWidget*>(mainWidget());
    QString name = m->nameEdit->text();
    if ( name.isEmpty() ) {
        KMessageBox::sorry(this, i18n("Please specify a name for the new configuration"));
        return;
    }

    if ( m->createRadio->isChecked() ) {
        if ( !configurationsProvider->createConfiguration(name,
              installationsProvider->rowToId(m->comboBox->currentIndex())) )
            return; // do not close the dialog. an error message should have been already shown.
    } else {
        //TODO implement me
        KMessageBox::information(this, "Copying is not implemented yet.");
        return;
    }
    
    KDialog::accept();
}

#include "createconfigurationdialog.moc"
