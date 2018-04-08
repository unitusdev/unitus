// Copyright (c) 2011-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "overviewpage.h"
#include "ui_overviewpage.h"

#include "bitcoinunits.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"

#include <QAbstractItemDelegate>
#include <QPainter>

#define DECORATION_SIZE 47//54
#define NUM_ITEMS 5

class TxViewDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    TxViewDelegate(const PlatformStyle *_platformStyle, QObject *parent=nullptr):
        QAbstractItemDelegate(parent), unit(BitcoinUnits::BTC),
        platformStyle(_platformStyle)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        QIcon icon = qvariant_cast<QIcon>(index.data(TransactionTableModel::RawDecorationRole));
        QRect mainRect = option.rect;
        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        QRect contentRect = QRect(mainRect.left() + 10, mainRect.top() + 10, mainRect.width() - 20, mainRect.height() - 20);
        path.addRoundedRect(contentRect, 5, 5);
        painter->fillPath(path, QBrush(QColor(37, 50, 82)));
        QPoint iconTopleft = QPoint(contentRect.left(), contentRect.top() + (contentRect.height() - DECORATION_SIZE) / 2);
        QRect decorationRect(iconTopleft, QSize(DECORATION_SIZE, DECORATION_SIZE));
        int xspace = DECORATION_SIZE + 15;
        int ypad = 6;
        int halfheight = (mainRect.height() - 2*ypad)/2;
        QRect dateRect(mainRect.left() + xspace + 9, mainRect.top()+ypad, mainRect.width() - xspace, halfheight);
        QRect addressRect(mainRect.left() + xspace, mainRect.top()+ypad+halfheight, mainRect.width() - xspace, halfheight);
        icon = QIcon(icon);
        icon.paint(painter, decorationRect);

        QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
        QString address = index.data(Qt::DisplayRole).toString();
        qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
        bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
        QVariant value = index.data(Qt::ForegroundRole);
        QColor foreground = option.palette.color(QPalette::Text);
        if(value.canConvert<QBrush>())
        {
            QBrush brush = qvariant_cast<QBrush>(value);
            foreground = brush.color();
        }

//        painter->setPen(foreground);
        painter->setPen(option.palette.color(QPalette::Text));
        QRect boundingRect;
        painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address, &boundingRect);

        if (index.data(TransactionTableModel::WatchonlyRole).toBool())
        {
            QIcon iconWatchonly = qvariant_cast<QIcon>(index.data(TransactionTableModel::WatchonlyDecorationRole));
            QRect watchonlyRect(boundingRect.right() + 5, mainRect.top()+ypad+halfheight, 16, halfheight);
            iconWatchonly.paint(painter, watchonlyRect);
        }

        if(amount < 0)
        {
            foreground = COLOR_NEGATIVE;
        }
        else if(!confirmed)
        {
            foreground = QColor(24, 141, 205);//COLOR_UNCONFIRMED;
        }
        else
        {
            foreground = QColor(16, 191, 119);//option.palette.color(QPalette::Text);
        }
        painter->setPen(foreground);
        QFont w_orgFont = painter->font();
        QFont w_newFont = w_orgFont;
        w_newFont.setPixelSize(25);
        painter->setFont(w_newFont);
        QString amountText = BitcoinUnits::formatWithComma(unit, amount, true, BitcoinUnits::separatorAlways);
        if(!confirmed)
        {
            amountText = QString("[") + amountText + QString("]");
        }
        QRect amountRect1 = contentRect;
        amountRect1.setWidth(contentRect.width() - 25);
        painter->drawText(amountRect1, Qt::AlignRight|Qt::AlignVCenter, amountText);
        painter->setFont(w_orgFont);

        painter->setPen(option.palette.color(QPalette::Text));
        painter->drawText(dateRect, Qt::AlignLeft|Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(DECORATION_SIZE + 50, DECORATION_SIZE + 50);
    }

    int unit;
    const PlatformStyle *platformStyle;

};
#include "overviewpage.moc"

OverviewPage::OverviewPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OverviewPage),
    clientModel(0),
    walletModel(0),
    currentBalance(-1),
    currentUnconfirmedBalance(-1),
    currentImmatureBalance(-1),
    currentWatchOnlyBalance(-1),
    currentWatchUnconfBalance(-1),
    currentWatchImmatureBalance(-1),
    txdelegate(new TxViewDelegate(platformStyle, this))
{
    ui->setupUi(this);
    //this->setObjectName("OverviewMainPage");

    // use a SingleColorIcon for the "out of sync warning" icon
    QIcon icon = platformStyle->SingleColorIcon(":/icons/warning");
    icon.addPixmap(icon.pixmap(QSize(64,64), QIcon::Normal), QIcon::Disabled); // also set the disabled icon because we are using a disabled QPushButton to work around missing HiDPI support of QLabel (https://bugreports.qt.io/browse/QTBUG-42503)
    ui->labelTransactionsStatus->setIcon(icon);
    ui->labelWalletStatus->setIcon(icon);

    // Recent transactions
    ui->listTransactions->setItemDelegate(txdelegate);
    ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listTransactions->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->listTransactions->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));

    // start with displaying the "out of sync" warnings
    showOutOfSyncWarning(true);
    connect(ui->labelWalletStatus, SIGNAL(clicked()), this, SLOT(handleOutOfSyncWarningClicks()));
    connect(ui->labelTransactionsStatus, SIGNAL(clicked()), this, SLOT(handleOutOfSyncWarningClicks()));
    connect(ui->btnLockWallet, SIGNAL(clicked(bool)), this, SLOT(btnLockWalletClicked(bool)));
}

void OverviewPage::handleTransactionClicked(const QModelIndex &index)
{
    if(filter)
        Q_EMIT transactionClicked(filter->mapToSource(index));
}

void OverviewPage::handleOutOfSyncWarningClicks()
{
    Q_EMIT outOfSyncWarningClicked();
}
void OverviewPage::btnLockWalletClicked(bool p_flag)
{
    Q_EMIT lockWallet(true);
}

OverviewPage::~OverviewPage()
{
    delete ui;
}

void OverviewPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance)
{
    int unit = walletModel->getOptionsModel()->getDisplayUnit();
    currentBalance = balance;
    currentUnconfirmedBalance = unconfirmedBalance;
    currentImmatureBalance = immatureBalance;
    currentWatchOnlyBalance = watchOnlyBalance;
    currentWatchUnconfBalance = watchUnconfBalance;
    currentWatchImmatureBalance = watchImmatureBalance;
    ui->labelBalance->setText(BitcoinUnits::formatWithComma(unit, balance));
    ui->labelUnconfirmed->setText(BitcoinUnits::formatWithComma(unit, unconfirmedBalance));
    ui->labelImmature->setText(BitcoinUnits::formatWithComma(unit, immatureBalance));
    ui->labelTotal->setText(BitcoinUnits::formatWithComma(unit, balance + unconfirmedBalance + immatureBalance));
    ui->labelWatchAvailable->setText(BitcoinUnits::formatWithComma(unit, watchOnlyBalance));
    ui->labelWatchPending->setText(BitcoinUnits::formatWithComma(unit, watchUnconfBalance));
    ui->labelWatchImmature->setText(BitcoinUnits::formatWithComma(unit, watchImmatureBalance));
    ui->labelWatchTotal->setText(BitcoinUnits::formatWithComma(unit, watchOnlyBalance + watchUnconfBalance + watchImmatureBalance));

    ui->labelAvailUnitBalance->setText(BitcoinUnits::nameOfUnit(unit));
    ui->labelAvailUnitImmature->setText(BitcoinUnits::nameOfUnit(unit));
    ui->labelAvailUnitUnconfirmed->setText(BitcoinUnits::nameOfUnit(unit));
    ui->labelAvailUnitTotal->setText(BitcoinUnits::nameOfUnit(unit));
    ui->labelAvailUnitWAvailable->setText(BitcoinUnits::nameOfUnit(unit));
    ui->labelAvailUnitWPending->setText(BitcoinUnits::nameOfUnit(unit));
    ui->labelAvailUnitWImmature->setText(BitcoinUnits::nameOfUnit(unit));
    ui->labelAvailUnitWTotal->setText(BitcoinUnits::nameOfUnit(unit));

    //    ui->labelBalance->setText(BitcoinUnits::formatWithUnit(unit, balance, false, BitcoinUnits::separatorAlways));
    //    ui->labelUnconfirmed->setText(BitcoinUnits::formatWithUnit(unit, unconfirmedBalance, false, BitcoinUnits::separatorAlways));
    //    ui->labelImmature->setText(BitcoinUnits::formatWithUnit(unit, immatureBalance, false, BitcoinUnits::separatorAlways));
    //    ui->labelTotal->setText(BitcoinUnits::formatWithUnit(unit, balance + unconfirmedBalance + immatureBalance, false, BitcoinUnits::separatorAlways));
    //    ui->labelWatchAvailable->setText(BitcoinUnits::formatWithUnit(unit, watchOnlyBalance, false, BitcoinUnits::separatorAlways));
    //    ui->labelWatchPending->setText(BitcoinUnits::formatWithUnit(unit, watchUnconfBalance, false, BitcoinUnits::separatorAlways));
    //    ui->labelWatchImmature->setText(BitcoinUnits::formatWithUnit(unit, watchImmatureBalance, false, BitcoinUnits::separatorAlways));
    //    ui->labelWatchTotal->setText(BitcoinUnits::formatWithUnit(unit, watchOnlyBalance + watchUnconfBalance + watchImmatureBalance, false, BitcoinUnits::separatorAlways));

    // only show immature (newly mined) balance if it's non-zero, so as not to complicate things
    // for the non-mining users
    bool showImmature = immatureBalance != 0;
    bool showWatchOnlyImmature = watchImmatureBalance != 0;

    // for symmetry reasons also show immature label when the watch-only one is shown
    ui->labelImmature->setVisible(showImmature || showWatchOnlyImmature);
    ui->labelImmatureText->setVisible(showImmature || showWatchOnlyImmature);
    ui->labelAvailUnitImmature->setVisible(showImmature || showWatchOnlyImmature);
    ui->labelWatchImmature->setVisible(showWatchOnlyImmature); // show watch-only immature balance
    ui->labelAvailUnitWImmature->setVisible(showWatchOnlyImmature);
}

// show/hide watch-only labels
void OverviewPage::updateWatchOnlyLabels(bool showWatchOnly)
{
    ui->labelSpendable->setVisible(showWatchOnly);      // show spendable label (only when watch-only is active)
    ui->labelWatchonly->setVisible(showWatchOnly);      // show watch-only label
    ui->lineWatchBalance->setVisible(showWatchOnly);    // show watch-only balance separator line
    ui->labelWatchAvailable->setVisible(showWatchOnly); // show watch-only available balance
    ui->labelWatchPending->setVisible(showWatchOnly);   // show watch-only pending balance
    ui->labelWatchTotal->setVisible(showWatchOnly);     // show watch-only total balance
    ui->labelAvailUnitWAvailable->setVisible(showWatchOnly);
    ui->labelAvailUnitWPending->setVisible(showWatchOnly);
    ui->labelAvailUnitWTotal->setVisible(showWatchOnly);

    if (!showWatchOnly)
    {
        ui->labelWatchImmature->hide();
        ui->labelAvailUnitWImmature->hide();
    }
}
#ifdef ENABLE_WALLET
void OverviewPage::setEncryptionStatus(int status)
{
    ui->walletUnlockedLabel->hide();
    ui->walletUnlockedIcon->hide();
    switch(status)
    {
    case WalletModel::Unencrypted:
//        ui->walletUnlockedLabel->hide();
//        ui->walletUnlockedIcon->hide();
        ui->btnLockWallet->setEnabled(true);
        break;
    case WalletModel::Unlocked:
//        ui->walletUnlockedLabel->show();
//        ui->walletUnlockedIcon->show();
        ui->btnLockWallet->show();
//        ui->walletUnlockedLabel->setText("Wallet unlocked");
//        ui->walletUnlockedIcon->setPixmap(QIcon(":/icons/lock_open").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        ui->btnLockWallet->setIcon(QIcon(":/icons/lock_open"));
        ui->btnLockWallet->setText("Lock Wallet");
        ui->btnLockWallet->setEnabled(false);
        break;
    case WalletModel::Locked:
//        ui->walletUnlockedLabel->show();
//        ui->walletUnlockedIcon->show();
        ui->btnLockWallet->show();
//        ui->walletUnlockedLabel->setText("Wallet locked");
//        ui->walletUnlockedIcon->setPixmap(QIcon(":/icons/lock_closed").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        ui->btnLockWallet->setIcon(QIcon(":/icons/lock_closed"));
        ui->btnLockWallet->setText("Your wallet is locked");
        ui->btnLockWallet->setEnabled(false);
        break;
    }
}
#endif
void OverviewPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
        // Show warning if this is a prerelease version
        connect(model, SIGNAL(alertsChanged(QString)), this, SLOT(updateAlerts(QString)));
        updateAlerts(model->getStatusBarWarnings());
    }
}

void OverviewPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
        // Set up transaction list
        filter.reset(new TransactionFilterProxy());
        filter->setSourceModel(model->getTransactionTableModel());
        filter->setLimit(NUM_ITEMS);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Date, Qt::DescendingOrder);

        ui->listTransactions->setModel(filter.get());
        ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);

        // Keep up to date with wallet
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance(),
                   model->getWatchBalance(), model->getWatchUnconfirmedBalance(), model->getWatchImmatureBalance());
        connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this, SLOT(setBalance(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)));

        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

        updateWatchOnlyLabels(model->haveWatchOnly());
        connect(model, SIGNAL(notifyWatchonlyChanged(bool)), this, SLOT(updateWatchOnlyLabels(bool)));
    }

    // update the display unit, to not use the default ("BTC")
    updateDisplayUnit();
}

void OverviewPage::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
        if(currentBalance != -1)
            setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance,
                       currentWatchOnlyBalance, currentWatchUnconfBalance, currentWatchImmatureBalance);

        // Update txdelegate->unit with the current unit
        txdelegate->unit = walletModel->getOptionsModel()->getDisplayUnit();

        ui->listTransactions->update();
    }
}

void OverviewPage::updateAlerts(const QString &warnings)
{
    this->ui->labelAlerts->setVisible(!warnings.isEmpty());
    this->ui->labelAlerts->setText(warnings);
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
    ui->labelWalletStatus->setVisible(fShow);
    ui->labelTransactionsStatus->setVisible(fShow);
}
