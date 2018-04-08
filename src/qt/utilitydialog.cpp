// Copyright (c) 2011-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "utilitydialog.h"

#include "ui_helpmessagedialog.h"

#include "bitcoingui.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "intro.h"
#include "paymentrequestplus.h"
#include "guiutil.h"

#include "clientversion.h"
#include "init.h"
#include "util.h"

#include <stdio.h>

#include <QCloseEvent>
#include <QLabel>
#include <QRegExp>
#include <QTextTable>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QPushButton>

/** "Help message" or "About" dialog box or "AboutQt" dialog box*/
HelpMessageDialog::HelpMessageDialog(QWidget *parent, int type) :
    QDialog(parent),
    ui(new Ui::HelpMessageDialog)
{
    ui->setupUi(this);

    QString version = tr(PACKAGE_NAME) + " " + tr("version") + " " + QString::fromStdString(FormatFullVersion());
    /* On x86 add a bit specifier to the version so that users can distinguish between
     * 32 and 64 bit builds. On other architectures, 32/64 bit may be more ambiguous.
     */
#if defined(__x86_64__)
    version += " " + tr("(%1-bit)").arg(64);
#elif defined(__i386__ )
    version += " " + tr("(%1-bit)").arg(32);
#endif

    if (type == 1)
    {
        setWindowTitle(tr("About %1").arg(tr(PACKAGE_NAME)));

        /// HTML-format the license message from the core
        QString licenseInfo = QString::fromStdString(LicenseInfo());
        QString licenseInfoHTML = licenseInfo;
        // Make URLs clickable
        QRegExp uri("<(.*)>", Qt::CaseSensitive, QRegExp::RegExp2);
        uri.setMinimal(true); // use non-greedy matching
        licenseInfoHTML.replace(uri, "<a style = 'color:#acb6cd' href=\"\\1\">\\1</a>");
        // Replace newlines with HTML breaks
        licenseInfoHTML.replace("\n", "<br>");

        ui->aboutMessage->setTextFormat(Qt::RichText);
        ui->scrollAreaAbout->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        text = version + "\n" + licenseInfo;
        ui->aboutMessage->setText(version + "<br><br>" + licenseInfoHTML);
        ui->aboutMessage->setWordWrap(true);
        ui->helpMessage->setVisible(false);
    } else if (type == 0){
        setWindowTitle(tr("Command-line options"));
        QString header = tr("Usage:") + "\n" +
            "  unitus-qt [" + tr("command-line options") + "]                     " + "\n";
        QTextCursor cursor(ui->helpMessage->document());
        QTextCharFormat charFormat;
        charFormat.setForeground( QBrush( QColor( "#9ca6bd" ) ) );
        cursor.setCharFormat( charFormat );
        cursor.insertText(version);
        cursor.insertBlock();
        cursor.insertText(header);
        cursor.insertBlock();

        std::string strUsage = HelpMessage(HMM_BITCOIN_QT);
        const bool showDebug = GetBoolArg("-help-debug", false);
        strUsage += HelpMessageGroup(tr("UI Options:").toStdString());
        if (showDebug) {
            strUsage += HelpMessageOpt("-allowselfsignedrootcertificates", strprintf("Allow self signed root certificates (default: %u)", DEFAULT_SELFSIGNED_ROOTCERTS));
        }
        strUsage += HelpMessageOpt("-choosedatadir", strprintf(tr("Choose data directory on startup (default: %u)").toStdString(), DEFAULT_CHOOSE_DATADIR));
        strUsage += HelpMessageOpt("-lang=<lang>", tr("Set language, for example \"de_DE\" (default: system locale)").toStdString());
        strUsage += HelpMessageOpt("-min", tr("Start minimized").toStdString());
        strUsage += HelpMessageOpt("-rootcertificates=<file>", tr("Set SSL root certificates for payment request (default: -system-)").toStdString());
        strUsage += HelpMessageOpt("-splash", strprintf(tr("Show splash screen on startup (default: %u)").toStdString(), DEFAULT_SPLASHSCREEN));
        strUsage += HelpMessageOpt("-resetguisettings", tr("Reset all settings changed in the GUI").toStdString());
        if (showDebug) {
            strUsage += HelpMessageOpt("-uiplatform", strprintf("Select platform to customize UI for (one of windows, macosx, other; default: %s)", BitcoinGUI::DEFAULT_UIPLATFORM));
        }
        QString coreOptions = QString::fromStdString(strUsage);
        text = version + "\n" + header + "\n" + coreOptions;

        QTextTableFormat tf;
        tf.setBorderStyle(QTextFrameFormat::BorderStyle_None);
        tf.setCellPadding(2);
        QVector<QTextLength> widths;
        widths << QTextLength(QTextLength::PercentageLength, 35);
        widths << QTextLength(QTextLength::PercentageLength, 65);
        tf.setColumnWidthConstraints(widths);

        QTextCharFormat bold;
        bold.setFontWeight(QFont::Bold);
        bold.setForeground( QBrush( QColor( "#9ca6bd" ) ) );
        

        Q_FOREACH (const QString &line, coreOptions.split("\n")) {
            if (line.startsWith("  -"))
            {
                cursor.currentTable()->appendRows(1);
                cursor.movePosition(QTextCursor::PreviousCell);
                cursor.movePosition(QTextCursor::NextRow);
                cursor.insertText(line.trimmed(), charFormat);
                cursor.movePosition(QTextCursor::NextCell);
            } else if (line.startsWith("   ")) {
                cursor.insertText(line.trimmed()+' ', charFormat);
            } else if (line.size() > 0) {
                //Title of a group
                if (cursor.currentTable())
                    cursor.currentTable()->appendRows(1);
                cursor.movePosition(QTextCursor::Down);
                cursor.insertText(line.trimmed(), bold);
                cursor.insertTable(1, 2, tf);
            }
        }

        ui->helpMessage->moveCursor(QTextCursor::Start);
        ui->scrollAreaAbout->setVisible(false);
        ui->aboutLogo->setVisible(false);
    }
    else{
        setWindowTitle(tr("About Qt"));
        ui->aboutLogo->setPixmap(QIcon(":/icons/aboutqt").pixmap(100,100));

        /// HTML-format the license message from the core
        QString aboutQtInfo = tr("<strong>About Qt</strong><br><br>") +
                tr("This program uses Qt version 5.7.1.<br><br>") +
                tr("Qt is a C++ toolkit for cross-platform application development.<br><br>") +
                tr("Qt provides single-source portability across all major desktop operating systems. It is also available for embedded Linux and other embedded and mobile operating systems.<br><br>") +
                tr("Qt is available under three different licensing options designed to accommodate the needs of our various users.<br><br>") +
                tr("Qt licensed under our commercial license agreement is appropriate for development of proprietary/commercial software where you do not want to share any source code with third parties or otherwise cannot comply with the terms of the GNU LGPL version 3.<br><br>") +
                tr("Qt licensed under the GNU LGPL version 3 is appropriate for the development of Qt applications provided you can comply with the terms and conditions of the GNU LGPL version 3.<br><br>") +
                tr("Please see <a style = 'color:#acb6cd' href='https://www1.qt.io/licensing/'>qt.io/licensing</a> for an overview of Qt licensing.<br><br>") +
                tr("Copyright (C) 2016 The Qt Company Ltd and other contributors.<br><br>") +
                tr("Qt and the Qt logo are trademarks of The Qt Company Ltd.<br><br>") +
                tr("Qt is The Qt Company Ltd product developed as an open source project. See <a style = 'color:#acb6cd' href='https://www.qt.io/'>qt.io</a> for more information.");

        QString aboutQtHTML = aboutQtInfo;
        // Make URLs clickable
//        QRegExp uri("<(.*)>", Qt::CaseSensitive, QRegExp::RegExp2);
//        uri.setMinimal(true); // use non-greedy matching
//        aboutQtHTML.replace(uri, "<a style = 'color:#acb6cd' href=\"\\1\">\\1</a>");
        // Replace newlines with HTML breaks
//        aboutQtHTML.replace("\n", "<br>");

        ui->aboutMessage->setTextFormat(Qt::RichText);
        ui->scrollAreaAbout->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        ui->aboutMessage->setText(aboutQtHTML);
        ui->aboutMessage->setWordWrap(true);
        ui->helpMessage->setVisible(false);
    }
    QPushButton * btnOk = ui->okButton->button(QDialogButtonBox::Ok);
    btnOk->setStyleSheet(QString("min-width: 120px; min-height: 35px; color:white;background-color: #188dcd;border-radius: 2px;border: 1px solid #188dcd;"));
}

HelpMessageDialog::~HelpMessageDialog()
{
    delete ui;
}

void HelpMessageDialog::printToConsole()
{
    // On other operating systems, the expected action is to print the message to the console.
    fprintf(stdout, "%s\n", qPrintable(text));
}

void HelpMessageDialog::showOrPrint()
{
#if defined(WIN32)
    // On Windows, show a message box, as there is no stderr/stdout in windowed applications
    exec();
#else
    // On other operating systems, print help text to console
    printToConsole();
#endif
}

void HelpMessageDialog::on_okButton_accepted()
{
    close();
}


/** "Shutdown" window */
ShutdownWindow::ShutdownWindow(QWidget *parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    QLabel * labelShutdown = new QLabel(
                tr("%1 is shutting down...").arg(tr(PACKAGE_NAME)) + "<br /><br />" +
                tr("Do not shut down the computer until this window disappears."));
    layout->addWidget(labelShutdown);
    setLayout(layout);
    labelShutdown->setStyleSheet(QString("color: #9ca6bd;"));
}

QWidget *ShutdownWindow::showShutdownWindow(BitcoinGUI *window)
{
    if (!window)
        return nullptr;

    // Show a simple window indicating shutdown status
    QWidget *shutdownWindow = new ShutdownWindow();
    shutdownWindow->setWindowTitle(window->windowTitle());
    shutdownWindow->setStyleSheet(QString("QWidget{background-image: url(:/backgrounds/headerbg);border: 1px;padding: 25px;}"));
    shutdownWindow->setStyleSheet(QString("QLabel{color: #212f4e;}"));

    // Center shutdown window at where main window was
    const QPoint global = window->mapToGlobal(window->rect().center());
    shutdownWindow->move(global.x() - shutdownWindow->width() / 2, global.y() - shutdownWindow->height() / 2);
    shutdownWindow->show();
    return shutdownWindow;
}

void ShutdownWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
}
