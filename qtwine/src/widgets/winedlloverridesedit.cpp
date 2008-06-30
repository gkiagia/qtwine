/***************************************************************************
 *   Copyright (C) 2008 by George Kiagiadakis <gkiagia@users.sf.net>       *
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
#include "winedlloverridesedit.h"

#include <QPair>
#include <QList>
#include <QLinkedList>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QApplication>
#include <QDesktopWidget>

#include <KLineEdit>
#include <KComboBox>
#include <KPushButton>
#include <KStandardGuiItem>
#include <KHBox>
#include <KLocalizedString>

using namespace QtWine;

class Line : public KHBox
{
public:
    Line(const QPair<QString, WineDllOverrides::DllOverrideType> & o, QWidget *parent);

    QPair<QString, WineDllOverrides::DllOverrideType> override() const;

    KLineEdit *lineEdit;
    KPushButton *rmButton;

private:
    void setupUi();
    KComboBox *comboBox;
};

Line::Line(const QPair<QString, WineDllOverrides::DllOverrideType> & o, QWidget *parent)
    : KHBox(parent)
{
    setupUi();
    if ( !o.first.isEmpty() ) {
        lineEdit->setText(o.first);
        comboBox->setCurrentIndex(static_cast<int>(o.second));
    } else
        lineEdit->setClickMessage(i18n("Enter dll name here"));
}

QPair<QString, WineDllOverrides::DllOverrideType> Line::override() const
{
    QPair<QString, WineDllOverrides::DllOverrideType> p;
    p.first = lineEdit->text();
    p.second = static_cast<WineDllOverrides::DllOverrideType>(comboBox->currentIndex());
    return p;
}

void Line::setupUi()
{
    lineEdit = new KLineEdit(this);
    comboBox = new KComboBox(this);
    comboBox->addItem(i18n("Disabled"));
    comboBox->addItem(i18n("Native"));
    comboBox->addItem(i18n("Builtin"));
    comboBox->addItem(i18n("Try first native, then builtin"));
    comboBox->addItem(i18n("Try first builtin, then native"));
    rmButton = new KPushButton(KStandardGuiItem::remove(), this);
}

struct WineDllOverridesEditPrivate
{
    QWidget *centralWidget;
    QList<Line*> lines;
    QSignalMapper rmButtonsMapper;
    KPushButton *addButton;
    QVBoxLayout *layout;
};


WineDllOverridesEdit::WineDllOverridesEdit(QWidget *parent)
    : QScrollArea(parent), d(new WineDllOverridesEditPrivate)
{
    d->centralWidget = new QWidget(this);
    setWidget(d->centralWidget);
    d->layout = new QVBoxLayout(d->centralWidget);
    d->addButton = new KPushButton(KStandardGuiItem::add(), this);
    d->layout->addWidget(d->addButton);
    connect(&d->rmButtonsMapper, SIGNAL(mapped(QWidget*)), this, SLOT(removeLine(QWidget*)) );
    connect(d->addButton, SIGNAL(clicked()), this, SLOT(addEmptyLine()) );
}

WineDllOverridesEdit::~WineDllOverridesEdit()
{
    delete d;
}

QString WineDllOverridesEdit::dllOverridesString() const
{
    return (QString) dllOverrides();
}

void WineDllOverridesEdit::setDllOverridesString(const QString & overrides)
{
    setDllOverrides(WineDllOverrides(overrides));
}

WineDllOverrides WineDllOverridesEdit::dllOverrides() const
{
    WineDllOverrides o;
    foreach(Line *l, d->lines)
        o << l->override();
    return o;
}

void WineDllOverridesEdit::setDllOverrides(const WineDllOverrides & overrides)
{
    // delete previous entries
    while ( !d->lines.isEmpty() ) {
        Line *line = d->lines.takeFirst();
        d->layout->removeWidget(line);
        line->deleteLater();
    }

    // load new entries
    QLinkedList< QPair<QString, WineDllOverrides::DllOverrideType> > list;
    QLinkedList< QPair<QString, WineDllOverrides::DllOverrideType> >::const_iterator i;
    list = overrides.overridesList();
    i = list.constBegin();
    while ( i != list.constEnd() ) {
        addLine(*i);
        ++i;
    }
}

void WineDllOverridesEdit::addEmptyLine()
{
    addLine(qMakePair(QString(), WineDllOverrides::Disabled));
}

void WineDllOverridesEdit::addLine(const QPair<QString, WineDllOverrides::DllOverrideType> & o)
{
    Line *newLine = new Line(o, d->centralWidget);
    d->lines.append(newLine);
    d->layout->insertWidget(d->lines.size()-1, newLine);
    d->rmButtonsMapper.setMapping(newLine->rmButton, newLine);
    connect(newLine->rmButton, SIGNAL(clicked()), &d->rmButtonsMapper, SLOT(map()));
    connect(newLine->lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(overridesChanged()) );
    QMetaObject::invokeMethod(this, "resizeCentralWidget", Qt::QueuedConnection);
    updateGeometry(); // autoresize container if this is the first line added.
}

void WineDllOverridesEdit::removeLine(QWidget *line)
{
    d->lines.removeAt(d->lines.indexOf(static_cast<Line*>(line)));
    d->layout->removeWidget(line);
    line->deleteLater();
    QMetaObject::invokeMethod(this, "resizeCentralWidget", Qt::QueuedConnection);
    emit overridesChanged();
}

void WineDllOverridesEdit::resizeEvent(QResizeEvent *event)
{
    resizeCentralWidget();
    QScrollArea::resizeEvent(event);
}

void WineDllOverridesEdit::resizeCentralWidget()
{
    // set the widget's size to be have the width of the scroll area
    // and the height that the layout prefers.
    QSize s = viewport()->size();
    QSize wSize = d->centralWidget->size().expandedTo(s).boundedTo(s);
    wSize.setHeight(d->layout->minimumSize().height());
    d->centralWidget->resize(wSize);
    ensureWidgetVisible(d->addButton);
}

QSize WineDllOverridesEdit::sizeHint() const
{
    int min_height = 200;
    int max_height = qApp->desktop()->availableGeometry(this).height();
    int height = d->layout->minimumSize().height();
    return QSize(d->layout->sizeHint().width(), qBound(min_height, height, max_height));
}

QSize WineDllOverridesEdit::minimumSizeHint() const
{
    return QSize(d->layout->sizeHint().width(), d->addButton->sizeHint().height());
}

#include "winedlloverridesedit.moc"
