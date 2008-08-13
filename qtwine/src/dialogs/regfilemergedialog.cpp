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
#include "../qtwineapplication.h"

#include "regedit.h"

#include <QTextStream>
#include <QScrollBar>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <KMessageBox>
#include <KTemporaryFile>
#include <KIO/NetAccess>
#include <KLocalizedString>
#include <KTextBrowser>

using namespace QtWine;


RegfileMergeDialog::RegfileMergeDialog(const KUrl & file, QWidget *parent)
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
	comboBox->setModel(configurationsProvider->model());
	comboBox->setModelColumn(configurationsProvider->model()->fieldIndex("name"));
	hlay->addWidget(comboBox);

	resizeLayout(vlay, marginHint(), spacingHint());

	if ( !KIO::NetAccess::download(file, tempFile, 0) ) {
		KMessageBox::sorry(this, KIO::NetAccess::lastErrorString());
		enableButtonOk(false);
		return;
	}

	QFile regfile( tempFile );
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

void RegfileMergeDialog::accept()
{
	QFile regfile( tempFile );
	if ( !regfile.open(QIODevice::ReadOnly | QIODevice::Text) )
		Q_ASSERT(false); // the file will open because it was opened in the constructor... (see above)

	KTemporaryFile utf8File;
	if ( !utf8File.open() ) {
		KMessageBox::error(this, i18n("QtWine could not open a temporary file"
						" (maybe the disk is full?)") );
		QDialog::reject();
	}

	// we have to do this because windows regedit 5.0 or later uses utf16 for exported .reg files
	// and wine's regedit cannot read them in utf16 format
	QTextStream ts(&regfile);
	while ( !ts.atEnd() ) {
		utf8File.write( ts.readLine().toUtf8() );
		utf8File.write("\n");
	}

	QString utf8FileName = utf8File.fileName();
	regfile.close();
	utf8File.close();

	// START WINE
	WineConfiguration wcfg = configurationsProvider->configurationByModelRow(comboBox->currentIndex());

	bool ok = RegEdit::importRegFile(utf8FileName, wcfg);
	if ( ok )
		KMessageBox::information(this, i18n("The requested file was merged successfully"
						" in the selected configuration's registry.") );
	else
		KMessageBox::sorry(this, i18n("Merging the requested file has failed.") );
		//TODO give advice here to the user...

	KIO::NetAccess::removeTempFile(tempFile);
	QDialog::accept();
}

void RegfileMergeDialog::setConfiguration(uint id)
{
    setConfigurationByModelRow(configurationsProvider->idToRow(id));
}

void RegfileMergeDialog::setConfigurationByModelRow(int row)
{
	comboBox->setCurrentIndex(row);
}

#include "regfilemergedialog.moc"
