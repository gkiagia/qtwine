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
#include "iconrequesterbutton.h"
#include <QIcon>
#include <QImageReader>
#include <KLocalizedString>
#include <KFileDialog>

IconRequesterButton::IconRequesterButton(QWidget *parent)
    : QPushButton(parent)
{
    setIconSize(QSize(48,48));
    connect(this, SIGNAL(clicked()), this, SLOT(slotIconClicked()) );
}

QString IconRequesterButton::iconFileName() const
{
    return m_iconFileName;
}

void IconRequesterButton::setIconFileName(const QString & icon)
{
    m_iconFileName = icon;
    setIcon(QIcon(icon));
    emit iconChanged();
}

static inline QString filterForSupportedImageTypes()
{
    QList<QByteArray> list = QImageReader::supportedImageFormats();
    QString filter;
    QString defaultFilter;

    foreach(QByteArray a, list) {
        defaultFilter += QString("*.%1 ").arg(QString(a.toLower()));
        filter += QString("*.%1|%2(*.%1)\n")
                    .arg(QString(a.toLower()))
                    .arg( i18n("%1 images").arg(QString(a.toUpper())) );
    }
    defaultFilter.chop(1); //remove the last space
    defaultFilter += '|';
    defaultFilter += i18n("All supported image types\n");
    filter.prepend(defaultFilter);
    return filter;
}

void IconRequesterButton::slotIconClicked()
{
    QString icon = KFileDialog::getOpenFileName(KUrl(), filterForSupportedImageTypes(),
                                                this, i18n("Select an icon"));
    if ( !icon.isEmpty() )
        setIconFileName(icon);
}

QSize IconRequesterButton::sizeHint() const
{
    return QSize(64,64);
}

#include "iconrequesterbutton.moc"
