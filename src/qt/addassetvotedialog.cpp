#include <QMessageBox>

#include "addassetvotedialog.h"
#include "ui_addassetvotedialog.h"
#include "assetvotedialog.h"
#include "coinmetadata.h"
#include "guiutil.h"
#include "walletmodel.h"

using namespace std;

#define DEFAULT_CONFIRMATIONS (6)
#define DEFAULT_REQ_SIGNERS (2)
#define DEFAULT_TOTAL_SIGNERS (3)
#define DEFAULT_MAX_TRADE (100000000)

AddAssetVoteDialog::AddAssetVoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAssetVoteDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Add asset vote"));
}

AddAssetVoteDialog::~AddAssetVoteDialog()
{
    delete ui;
}

void AddAssetVoteDialog::updateIds()
{
    QComboBox *assetList = ui->assetList;
    int index = assetList->currentIndex();

    if(index == assetList->count() - 1)
    {
        // Custom asset
        ui->blockchainId->clear();
        ui->blockchainId->setReadOnly(false);
        ui->blockchainId->setEnabled(true);
        ui->assetId->clear();
        ui->assetId->setReadOnly(false);
        ui->assetId->setEnabled(true);
        ui->maxTradeCoins->setEnabled(false);
        ui->assetSymbol->clear();
    }
    else
    {
        uint64 globalId = assetList->itemData(index).toULongLong();
        int blockchainId = GetBlockchainId(globalId);
        int assetId = GetAssetId(globalId);
        QString assetSymbol = QString::fromStdString(GetAssetSymbol(globalId));

        ui->blockchainId->setText(QString::number(blockchainId));
        ui->blockchainId->setReadOnly(true);
        ui->blockchainId->setEnabled(false);
        ui->assetId->setText(QString::number(assetId));
        ui->assetId->setReadOnly(true);
        ui->assetId->setEnabled(false);
        ui->maxTradeCoins->setEnabled(true);
        ui->assetSymbol->setText(assetSymbol);
    }

    // Force update of maxTrade and maxTradeCoins fields
    ui->maxTrade->clear();
    ui->maxTrade->setText(QString::number(DEFAULT_MAX_TRADE));
}

void AddAssetVoteDialog::setModel(WalletModel *model)
{
    this->model = model;

    QComboBox *assetList = ui->assetList;
    assetList->clear();
    for(coin_metadata_map::const_iterator it = COIN_METADATA.begin(); it != COIN_METADATA.end(); it++)
    {
        uint64 globalId = it->first;
        QString assetName = QString::fromStdString(GetAssetName(globalId));
        QString assetSymbol = QString::fromStdString(GetAssetSymbol(globalId));
        assetList->addItem(assetName + " (" + assetSymbol + ")", globalId);
    }
    assetList->addItem(tr("other..."));

    ui->blockchainId->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->assetId->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->confirmations->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->requiredDepositSigners->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->totalDepositSigners->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->maxTrade->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->maxTradeCoins->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    updateIds();
    ui->confirmations->setText(QString::number(DEFAULT_CONFIRMATIONS));
    ui->requiredDepositSigners->setText(QString::number(DEFAULT_REQ_SIGNERS));
    ui->totalDepositSigners->setText(QString::number(DEFAULT_TOTAL_SIGNERS));
    ui->maxTrade->setText(QString::number(DEFAULT_MAX_TRADE));
}

void AddAssetVoteDialog::error(const QString &message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

void AddAssetVoteDialog::accept()
{
    bool ok;
    int nBlockchainId = ui->blockchainId->text().toInt(&ok);
    if(!ok || (nBlockchainId < 0) || (nBlockchainId > MAX_BLOCKCHAIN_ID))
    {
        error(tr("Invalid blockchain id"));
        return;
    }

    int nAssetId = ui->assetId->text().toInt(&ok);
    if(!ok || (nAssetId < 0) || (nAssetId > MAX_ASSET_ID))
    {
        error(tr("Invalid asset id"));
        return;
    }

    uint16_t nNumberOfConfirmations = ui->confirmations->text().toInt(&ok);
    if(!ok)
    {
        error(tr("Invalid number of confirmations"));
        return;
    }

    uint8_t nRequiredDepositSigners = ui->requiredDepositSigners->text().toInt(&ok);
    if(!ok || (nRequiredDepositSigners < MIN_REQ_SIGNERS))
    {
        error(tr("Invalid number of required deposit signers"));
        return;
    }

    uint8_t nTotalDepositSigners = ui->totalDepositSigners->text().toInt(&ok);
    if(!ok || (nTotalDepositSigners < MIN_TOTAL_SIGNERS))
    {
        error(tr("Invalid number of total deposit signers"));
        return;
    }

    uint64 nMaxTrade = ui->maxTrade->text().toULongLong(&ok);
    if(!ok)
    {
        error(tr("Invalid max trade"));
        return;
    }

    ((AssetVoteDialog *) parentWidget())->addTableRow(nBlockchainId,
                                                      nAssetId,
                                                      nNumberOfConfirmations,
                                                      nRequiredDepositSigners,
                                                      nTotalDepositSigners,
                                                      nMaxTrade);

    QDialog::accept();
}

void AddAssetVoteDialog::on_assetList_currentIndexChanged(int index)
{
    updateIds();
}

void AddAssetVoteDialog::on_maxTrade_textChanged(const QString &maxTrade)
{
    int index = ui->assetList->currentIndex();
    if(index == ui->assetList->count() - 1)
        return;

    uint64 globalId = ui->assetList->itemData(index).toULongLong();
    unsigned char exponent = GetAssetUnitExponent(globalId);
    bool ok;
    uint64 n = maxTrade.toULongLong(&ok);
    if(!ok)
        return;

    if(n != GUIUtil::coinsToUnits(ui->maxTradeCoins->text(), exponent))
    {
        QString maxTradeCoins = GUIUtil::unitsToCoins(n, exponent);
        ui->maxTradeCoins->setText(maxTradeCoins);
    }
}

void AddAssetVoteDialog::on_maxTradeCoins_textChanged(const QString &maxTradeCoins)
{
    int index = ui->assetList->currentIndex();
    if(index == ui->assetList->count() - 1)
        return;

    uint64 globalId = ui->assetList->itemData(index).toULongLong();
    unsigned char exponent = GetAssetUnitExponent(globalId);
    uint64 n = GUIUtil::coinsToUnits(maxTradeCoins, exponent);

    if(n != ui->maxTrade->text().toULongLong())
        ui->maxTrade->setText(QString::number(n));
}
