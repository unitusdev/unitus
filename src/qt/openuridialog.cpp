// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "openuridialog.h"
#include "ui_openuridialog.h"

#include "guiutil.h"
#include "walletmodel.h"

#include <QUrl>

OpenURIDialog::OpenURIDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenURIDialog)
{
    ui->setupUi(this);
#if QT_VERSION >= 0x040700
    ui->uriEdit->setPlaceholderText("bitcoin:");
#endif

#ifdef WIN32
    ui->buttonBox->setLayoutDirection(Qt::LeftToRight);
#else
    ui->buttonBox->setLayoutDirection(Qt::RightToLeft);
#endif

    QPushButton * okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setStyleSheet(QString("min-width: 120px; min-height: 37px; color:white;background-color: #188dcd;border-radius: 2px;border: 1px solid #188dcd;"));

    QPushButton * cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);
    cancelButton->setStyleSheet(QString("background-color:transparent; border:none; color:#188dcd; font-size: 14px; margin-right: 25px"));
    cancelButton->setIcon(QIcon(":/icons/arrow_right"));
    cancelButton->setLayoutDirection(Qt::RightToLeft);
}

OpenURIDialog::~OpenURIDialog()
{
    delete ui;
}

QString OpenURIDialog::getURI()
{
    return ui->uriEdit->text();
}

void OpenURIDialog::accept()
{
    SendCoinsRecipient rcp;
    if(GUIUtil::parseBitcoinURI(getURI(), &rcp))
    {
        /* Only accept value URIs */
        QDialog::accept();
    } else {
        ui->uriEdit->setValid(false);
    }
}

void OpenURIDialog::on_selectFileButton_clicked()
{
    QString filename = GUIUtil::getOpenFileName(this, tr("Select payment request file to open"), "", "", NULL);
    if(filename.isEmpty())
        return;
    QUrl fileUri = QUrl::fromLocalFile(filename);
    ui->uriEdit->setText("bitcoin:?r=" + QUrl::toPercentEncoding(fileUri.toString()));
}
