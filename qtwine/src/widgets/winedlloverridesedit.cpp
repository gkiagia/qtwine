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
    void setOverride(const QPair<QString, WineDllOverrides::DllOverrideType> & o);

    KLineEdit *lineEdit;
    KPushButton *rmButton;

private:
    KComboBox *comboBox;
};

Line::Line(const QPair<QString, WineDllOverrides::DllOverrideType> & o, QWidget *parent)
    : KHBox(parent)
{
    lineEdit = new KLineEdit(this);
    comboBox = new KComboBox(this);
    comboBox->addItem(i18n("Disabled"));
    comboBox->addItem(i18n("Native"));
    comboBox->addItem(i18n("Builtin"));
    comboBox->addItem(i18n("Try first native, then builtin"));
    comboBox->addItem(i18n("Try first builtin, then native"));
    rmButton = new KPushButton(KStandardGuiItem::remove(), this);
    setOverride(o);
}

QPair<QString, WineDllOverrides::DllOverrideType> Line::override() const
{
    QPair<QString, WineDllOverrides::DllOverrideType> p;
    p.first = lineEdit->text();
    p.second = static_cast<WineDllOverrides::DllOverrideType>(comboBox->currentIndex());
    return p;
}

void Line::setOverride(const QPair<QString, WineDllOverrides::DllOverrideType> & o)
{
    if ( !o.first.isEmpty() ) {
        lineEdit->setText(o.first);
        comboBox->setCurrentIndex(static_cast<int>(o.second));
    } else {
        lineEdit->setClickMessage(i18n("Enter dll name here"));
        comboBox->setCurrentIndex(0);
    }
}

class WineDllOverridesEdit::Private
{
public:
    Private(WineDllOverridesEdit *qq) : q(qq) {}
    void addEmptyLine();
    void addLine(const QPair<QString, WineDllOverrides::DllOverrideType> & o);
    void removeLine(QWidget *line);
    void resizeCentralWidget();

    QWidget *centralWidget;
    QList<Line*> lines;
    QSignalMapper rmButtonsMapper;
    KPushButton *addButton;
    QVBoxLayout *layout;
    WineDllOverridesEdit *q;
};


WineDllOverridesEdit::WineDllOverridesEdit(QWidget *parent)
    : QScrollArea(parent), d(new Private(this))
{
    d->centralWidget = new QWidget(this);
    setWidget(d->centralWidget);

    d->layout = new QVBoxLayout(d->centralWidget);
    d->addButton = new KPushButton(KStandardGuiItem::add(), d->centralWidget);
    d->layout->addWidget(d->addButton);

    /* this is mainly for having correct widget width
    from construction time (and for better usability) */
    d->addEmptyLine();

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
    // load new entries. first update all existing lines and add new lines if needed.
    bool newLinesAdded = false;
    QList<Line*>::iterator lineIterator;
    lineIterator = d->lines.begin();
    QLinkedList< QPair<QString, WineDllOverrides::DllOverrideType> > list;
    QLinkedList< QPair<QString, WineDllOverrides::DllOverrideType> >::const_iterator i;
    list = overrides.overridesList();
    i = list.constBegin();
    while ( i != list.constEnd() ) {
        if ( lineIterator != d->lines.end() ) {
            (*lineIterator)->setOverride(*i);
            ++lineIterator;
        } else {
            d->addLine(*i);
            newLinesAdded = true;
        }
        ++i;
    }

    //remove the rest of the lines
    while ( !newLinesAdded and lineIterator != d->lines.end() ) {
        d->layout->removeWidget(*lineIterator);
        (*lineIterator)->deleteLater();
        lineIterator = d->lines.erase(lineIterator);
    }
}

void WineDllOverridesEdit::Private::addEmptyLine()
{
    addLine(qMakePair(QString(), WineDllOverrides::Disabled));
}

void WineDllOverridesEdit::Private::addLine(const QPair<QString, WineDllOverrides::DllOverrideType> & o)
{
    Line *newLine = new Line(o, centralWidget);
    lines.append(newLine);
    layout->insertWidget(lines.size()-1, newLine);
    rmButtonsMapper.setMapping(newLine->rmButton, newLine);
    connect(newLine->rmButton, SIGNAL(clicked()), &rmButtonsMapper, SLOT(map()));
    connect(newLine->lineEdit, SIGNAL(textChanged(QString)), q, SIGNAL(overridesChanged()) );
    QMetaObject::invokeMethod(q, "resizeCentralWidget", Qt::QueuedConnection);
    q->updateGeometry(); // autoresize container if this is the first line added.
}

void WineDllOverridesEdit::Private::removeLine(QWidget *line)
{
    lines.removeAt(lines.indexOf(static_cast<Line*>(line)));
    layout->removeWidget(line);
    line->deleteLater();
    QMetaObject::invokeMethod(q, "resizeCentralWidget", Qt::QueuedConnection);
    emit q->overridesChanged();
}

void WineDllOverridesEdit::resizeEvent(QResizeEvent *event)
{
    d->resizeCentralWidget();
    QScrollArea::resizeEvent(event);
}

void WineDllOverridesEdit::Private::resizeCentralWidget()
{
    // set the widget's size to be have the width of the scroll area
    // and the height that the layout prefers.
    QSize s = q->viewport()->size();
    QSize wSize = centralWidget->size().expandedTo(s).boundedTo(s);
    wSize.setHeight(layout->minimumSize().height());
    centralWidget->resize(wSize);
    q->ensureWidgetVisible(addButton);
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
