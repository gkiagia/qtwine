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
#include "winedlloverridesrequester.h"
#include "winedlloverridesedit.h"

#include <QToolButton>

#include <KLineEdit>
#include <KDialog>
#include <KLocalizedString>

using namespace QtWine;

class WineDllOverridesRequesterPrivate
{
public:
    WineDllOverridesRequesterPrivate(WineDllOverridesRequester *qq) : q(qq) {}
    void toolButtonClicked();
    void lineEditTextChanged();

    KLineEdit *m_lineEdit;
    QToolButton *m_toolButton;
    WineDllOverrides m_overrides;
    WineDllOverridesRequester *q;
};

WineDllOverridesRequester::WineDllOverridesRequester(QWidget *parent)
    : KHBox(parent), d(new WineDllOverridesRequesterPrivate(this))
{
    d->m_lineEdit = new KLineEdit(this);
    d->m_toolButton = new QToolButton(this);
    d->m_toolButton->setIcon(KIcon("document-properties"));
    setSpacing(KDialog::spacingHint());
    connect(d->m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(lineEditTextChanged()) );
    connect(d->m_toolButton, SIGNAL(clicked()), this, SLOT(toolButtonClicked()) );
}

WineDllOverridesRequester::~WineDllOverridesRequester()
{
    delete d;
}

QString WineDllOverridesRequester::dllOverridesString() const
{
    return dllOverrides();
}

void WineDllOverridesRequester::setDllOverridesString(const QString & overridesString)
{
    setDllOverrides(WineDllOverrides(overridesString));
}

WineDllOverrides WineDllOverridesRequester::dllOverrides() const
{
    return d->m_overrides;
}

void WineDllOverridesRequester::setDllOverrides(const WineDllOverrides & overrides)
{
    d->m_overrides = overrides;
    d->m_lineEdit->setText((QString)overrides);
    emit overridesChanged();
}

void WineDllOverridesRequesterPrivate::toolButtonClicked()
{
    KDialog dialog;
    dialog.setCaption(KDialog::makeStandardCaption(i18n("Edit wine's dll overrides"), &dialog));
    WineDllOverridesEdit *edit = new WineDllOverridesEdit(&dialog);
    edit->setDllOverrides(m_overrides);
    dialog.setMainWidget(edit);
    edit->updateGeometry();
    if ( dialog.exec() == QDialog::Accepted )
        q->setDllOverrides(edit->dllOverrides());
}

void WineDllOverridesRequesterPrivate::lineEditTextChanged()
{
    WineDllOverrides v;
    if ( v.parseString(m_lineEdit->text()) )
        m_overrides = v;
}

#include "winedlloverridesrequester.moc"
