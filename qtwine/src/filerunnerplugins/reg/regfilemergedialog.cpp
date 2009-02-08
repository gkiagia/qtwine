/***************************************************************************
 *   Copyright (C) 2006-2008 by George Kiagiadakis                         *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "regfilemergedialog.h"
#include "../../qtwineapplication.h"

#include <QScrollBar>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>

#include <KMessageBox>
#include <KLocalizedString>
#include <KTextBrowser>

using namespace QtWine;


RegfileMergeDialog::RegfileMergeDialog(const QString & file, QWidget *parent)
	: KDialog(parent)
{
	setButtons(KDialog::Ok | KDialog::Cancel);
	setCaption(makeStandardCaption(i18n("Merge a registry file"), this));

	QWidget *w = new QWidget(this);
	setMainWidget(w);
	QVBoxLayout *vlay = new QVBoxLayout(w);

	KTextBrowser *textBrowser = new KTextBrowser(w);
	vlay->addWidget(textBrowser);

	QHBoxLayout *hlay = new QHBoxLayout;
	vlay->addLayout(hlay);

	QLabel *label = new QLabel(i18n("Merge with the registry of this wine configuration:"), w);
	label->setWordWrap(true);
	hlay->addWidget(label);

	comboBox = new QComboBox(w);
	comboBox->setModel(qtwineApp->wineConfigurationsModel());
	comboBox->setModelColumn(qtwineApp->wineConfigurationsModel()->fieldIndex("name"));
	hlay->addWidget(comboBox);

	resizeLayout(vlay, marginHint(), spacingHint());

	QFile regfile(file);
	if ( regfile.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		QTextStream ts(&regfile);
		while ( !ts.atEnd() )
			textBrowser->append( ts.readLine() );
		regfile.close();
	} else {
		KMessageBox::sorry(this, i18n("The specified .reg file could not"
					" be opened. Check that you have read permissions on it.") );
		enableButtonOk(false);
		return;
	}

	// scroll to top
	QMetaObject::invokeMethod(textBrowser->verticalScrollBar(), "setValue", Qt::QueuedConnection,
				  Q_ARG(int, textBrowser->verticalScrollBar()->minimum()) );
}

int RegfileMergeDialog::configurationId() const
{
    return qtwineApp->wineConfigurationsModel()->rowToId(comboBox->currentIndex());
}

void RegfileMergeDialog::setConfigurationId(int id)
{
    comboBox->setCurrentIndex(qtwineApp->wineConfigurationsModel()->idToRow(id));
}

#include "regfilemergedialog.moc"
