#include <QMessageBox>
#include <stdint.h>

#include "assetvotedialog.h"
#include "ui_assetvotedialog.h"
#include "addassetvotedialog.h"
#include "coinmetadata.h"
#include "guiutil.h"
#include "util.h"
#include "vote.h"
#include "walletmodel.h"

using namespace std;

AssetVoteDialog::AssetVoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AssetVoteDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Asset vote"));
    ui->assetVoteTable->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
}

AssetVoteDialog::~AssetVoteDialog()
{
    delete ui;
}

void AssetVoteDialog::setModel(WalletModel *model)
{
    this->model = model;
    fillAssetVoteTable();
}

void AssetVoteDialog::addTableRow(int blockchainId,
                                  int assetId,
                                  uint16_t confirmations,
                                  uint8_t requiredSigners,
                                  uint8_t totalSigners,
                                  uint64 maxTrade)
{
    QTableWidget *table = ui->assetVoteTable;
    int row = table->rowCount();

    table->setSortingEnabled(false);
    table->setRowCount(row + 1);

    uint64 globalId = AssetGlobalId(blockchainId, assetId);

    QTableWidgetItem *idItem = new QTableWidgetItem();
    idItem->setData(Qt::DisplayRole, globalId);
    idItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
    idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
    table->setItem(row, 0, idItem);

    QTableWidgetItem *assetNameItem = new QTableWidgetItem();
    QString assetName = QString::fromStdString(GetAssetName(globalId));
    QString assetSymbol = QString::fromStdString(GetAssetSymbol(globalId));
    QString assetStr = "";
    if(!assetName.isEmpty())
    {
        if(!assetSymbol.isEmpty())
            assetStr = assetName + " (" + assetSymbol + ")";
        else
            assetStr = assetName;
    }
    else if(!assetSymbol.isEmpty())
        assetStr = "(" + assetSymbol + ")";
    assetNameItem->setData(Qt::DisplayRole, assetStr);
    assetNameItem->setFlags(assetNameItem->flags() & ~Qt::ItemIsEditable);
    table->setItem(row, 1, assetNameItem);

    QTableWidgetItem *blockchainItem = new QTableWidgetItem();
    blockchainItem->setData(Qt::DisplayRole, blockchainId);
    blockchainItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
    blockchainItem->setFlags(blockchainItem->flags() & ~Qt::ItemIsEditable);
    table->setItem(row, 2, blockchainItem);

    QTableWidgetItem *assetItem = new QTableWidgetItem();
    assetItem->setData(Qt::DisplayRole, assetId);
    assetItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
    assetItem->setFlags(assetItem->flags() & ~Qt::ItemIsEditable);
    table->setItem(row, 3, assetItem);

    QTableWidgetItem *confirmationsItem = new QTableWidgetItem();
    confirmationsItem->setData(Qt::DisplayRole, confirmations);
    confirmationsItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
    table->setItem(row, 4, confirmationsItem);

    QTableWidgetItem *requiredSignersItem = new QTableWidgetItem();
    requiredSignersItem->setData(Qt::DisplayRole, requiredSigners);
    requiredSignersItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
    table->setItem(row, 5, requiredSignersItem);

    QTableWidgetItem *totalSignersItem = new QTableWidgetItem();
    totalSignersItem->setData(Qt::DisplayRole, totalSigners);
    totalSignersItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
    table->setItem(row, 6, totalSignersItem);

    QTableWidgetItem *maxTradeItem = new QTableWidgetItem();
    maxTradeItem->setData(Qt::DisplayRole, maxTrade);
    maxTradeItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
    table->setItem(row, 7, maxTradeItem);

    QTableWidgetItem *maxTradeCoinsItem = new QTableWidgetItem();
    QString maxTradeStr = "";
    if(!assetSymbol.isEmpty())
    {
        unsigned char exponent = GetAssetUnitExponent(globalId);
        maxTradeStr = GUIUtil::unitsToCoins(maxTrade, exponent);
        maxTradeStr = maxTradeStr + " " + assetSymbol;
        maxTradeCoinsItem->setFlags(maxTradeCoinsItem->flags() | Qt::ItemIsEditable);
    }
    else
        maxTradeCoinsItem->setFlags(maxTradeCoinsItem->flags() & ~Qt::ItemIsEditable);
    maxTradeCoinsItem->setData(Qt::DisplayRole, maxTradeStr);
    maxTradeCoinsItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));

    table->setItem(row, 8, maxTradeCoinsItem);

    table->setSortingEnabled(true);
}

void AssetVoteDialog::fillAssetVoteTable()
{
    CVote vote = model->getVote();
    QTableWidget *table = ui->assetVoteTable;
    table->setRowCount(0);

    for(int i = 0; i < vote.vAssetVote.size(); i++)
    {
        const CAssetVote &assetVote = vote.vAssetVote[i];

        addTableRow(assetVote.nBlockchainId,
                    assetVote.nAssetId,
                    assetVote.nNumberOfConfirmations,
                    assetVote.nRequiredDepositSigners,
                    assetVote.nTotalDepositSigners,
                    assetVote.GetMaxTrade());
    }

    table->setVisible(false);
    table->resizeColumnsToContents();
    table->setVisible(true);
}

void AssetVoteDialog::addVote()
{
    AddAssetVoteDialog dlg(this);
    dlg.setModel(model);
    dlg.exec();

    QTableWidget *table = ui->assetVoteTable;
    table->setVisible(false);
    table->resizeColumnsToContents();
    table->setVisible(true);
}

void AssetVoteDialog::removeVote()
{
    QTableWidget *table = ui->assetVoteTable;
    QItemSelection selection(table->selectionModel()->selection());

    QList<int> rows;
    foreach(const QModelIndex &index, selection.indexes())
    {
        rows.append(index.row());
    }
    qSort(rows);

    int prev = -1;
    for(int i = rows.count() - 1; i >= 0; i--)
    {
        int current = rows[i];
        if(current != prev)
        {
            table->removeRow(current);
            prev = current;
        }
    }
}

void AssetVoteDialog::on_addAssetVoteButton_clicked()
{
    addVote();
}

void AssetVoteDialog::on_removeAssetVoteButton_clicked()
{
    removeVote();
}

void AssetVoteDialog::error(const QString &message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

void AssetVoteDialog::accept()
{
    CVote vote = model->getVote();
    vector<CAssetVote> vAssetVote;
    QTableWidget *table = ui->assetVoteTable;
    int rows = table->rowCount();

    for(int i = 0; i < rows; i++)
    {
        int row = i + 1;
        QTableWidgetItem *idItem = table->item(i, 0);
        QTableWidgetItem *blockchainItem = table->item(i, 2);
        QTableWidgetItem *assetItem = table->item(i, 3);
        QTableWidgetItem *confirmationsItem = table->item(i, 4);
        QTableWidgetItem *requiredSignersItem = table->item(i, 5);
        QTableWidgetItem *totalSignersItem = table->item(i, 6);
        QTableWidgetItem *maxTradeItem = table->item(i, 7);

        if(!idItem && !blockchainItem && !assetItem && !confirmationsItem && !requiredSignersItem && !totalSignersItem && !maxTradeItem)
            continue;

        bool ok;
        int nBlockchainId = blockchainItem->text().toInt(&ok);
        if(!ok || (nBlockchainId < 0) || (nBlockchainId > MAX_BLOCKCHAIN_ID))
        {
            error(tr("Invalid blockchain id on row %1").arg(row));
            return;
        }

        int nAssetId = assetItem->text().toInt(&ok);
        if(!ok || (nAssetId < 0) || (nAssetId > MAX_ASSET_ID))
        {
            error(tr("Invalid asset id on row %1").arg(row));
            return;
        }

        uint64 globalId = idItem->text().toULongLong(&ok);
        if(!ok || (globalId != AssetGlobalId(nBlockchainId, nAssetId)))
        {
            error(tr("Invalid id on row %1").arg(row));
            return;
        }

        uint16_t nNumberOfConfirmations = confirmationsItem->text().toInt(&ok);
        if(!ok)
          {
            error(tr("Invalid number of confirmations"));
            return;
          }

        uint8_t nRequiredDepositSigners = requiredSignersItem->text().toInt(&ok);
        if(!ok || (nRequiredDepositSigners < MIN_REQ_SIGNERS))
        {
            error(tr("Invalid number of required deposit signers on row %1").arg(row));
            return;
        }

        uint8_t nTotalDepositSigners = totalSignersItem->text().toInt(&ok);
        if(!ok || (nTotalDepositSigners < MIN_TOTAL_SIGNERS))
        {
            error(tr("Invalid number of total deposit signers on row %1").arg(row));
            return;
        }

        uint64 nMaxTrade = maxTradeItem->text().toULongLong(&ok);
        uint8_t nMaxTradeExpParam = GetExponentialSeriesParameter(nMaxTrade);
        if(!ok || (nMaxTradeExpParam > EXP_SERIES_MAX_PARAM))
        {
            error(tr("Invalid max trade on row %1").arg(row));
            return;
        }

        CAssetVote assetVote;
        assetVote.nBlockchainId = nBlockchainId;
        assetVote.nAssetId = nAssetId;
        assetVote.nNumberOfConfirmations = nNumberOfConfirmations;
        assetVote.nRequiredDepositSigners = nRequiredDepositSigners;
        assetVote.nTotalDepositSigners = nTotalDepositSigners;
        assetVote.nMaxTradeExpParam = nMaxTradeExpParam;

        vAssetVote.push_back(assetVote);
    }

    vote.vAssetVote = vAssetVote;

    if(!vote.IsValid(model->getProtocolVersion()))
    {
        error(tr("The new vote is invalid"));
        return;
    }

    model->setVote(vote);

    QDialog::accept();
}

void AssetVoteDialog::on_assetVoteTable_cellChanged(int row, int column)
{
    QTableWidget *table = ui->assetVoteTable;
    QTableWidgetItem *idItem = table->item(row, 0);
    QTableWidgetItem *maxTradeItem = table->item(row, 7);
    QTableWidgetItem *maxTradeCoinsItem = table->item(row, 8);
    if((idItem == NULL) || (maxTradeItem == NULL) || (maxTradeCoinsItem == NULL))
        return;

    if(column == 7)
    {
        uint64 globalId = idItem->data(Qt::DisplayRole).toULongLong();
        QString assetSymbol = QString::fromStdString(GetAssetSymbol(globalId));
        if(assetSymbol.isEmpty())
        {
            maxTradeCoinsItem->setData(Qt::DisplayRole, QString(""));
        }
        else
        {
            uint64 maxTrade = maxTradeItem->data(Qt::DisplayRole).toULongLong();
            unsigned char exponent = GetAssetUnitExponent(globalId);
            QString maxTradeStr = maxTradeCoinsItem->text();
            maxTradeStr = GUIUtil::unitsToCoins(maxTrade, exponent);
            maxTradeStr = maxTradeStr + " " + assetSymbol;
            maxTradeCoinsItem->setData(Qt::DisplayRole, maxTradeStr);
        }
    }
    else if(column == 8)
    {
        uint64 globalId = idItem->data(Qt::DisplayRole).toULongLong();
        QString assetSymbol = QString::fromStdString(GetAssetSymbol(globalId));
        if(assetSymbol.isEmpty())
        {
            maxTradeCoinsItem->setData(Qt::DisplayRole, QString(""));
        }
        else
        {
            unsigned char exponent = GetAssetUnitExponent(globalId);
            QString maxTradeStr = maxTradeCoinsItem->text();
            maxTradeStr = maxTradeStr.left(maxTradeStr.indexOf(" "));
            uint64 n = GUIUtil::coinsToUnits(maxTradeStr, exponent);
            if(n != maxTradeItem->data(Qt::DisplayRole).toULongLong())
            {
                maxTradeItem->setData(Qt::DisplayRole, n);
            }
        }
    }
    else
        return;

    table->setVisible(false);
    table->resizeColumnsToContents();
    table->setVisible(true);
}
