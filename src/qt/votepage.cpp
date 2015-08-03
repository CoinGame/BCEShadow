#include <QTimer>

#include "votepage.h"
#include "ui_votepage.h"
#include "walletmodel.h"
#include "clientmodel.h"
#include "custodianvotedialog.h"
#include "motionvotedialog.h"
#include "feevotedialog.h"
#include "reputationvotedialog.h"
#include "datafeeddialog.h"
#include "guiconstants.h"
#include "optionsmodel.h"
#include "bitcoinunits.h"
#include "guiutil.h"

VotePage::VotePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VotePage),
    model(NULL),
    lastBestBlock(NULL)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(MODEL_UPDATE_DELAY);
}

VotePage::~VotePage()
{
    delete ui;
}

void VotePage::setModel(WalletModel *model)
{
    this->model = model;
}

void VotePage::setClientModel(ClientModel *clientModel)
{
    this->clientModel = clientModel;
}

void VotePage::on_custodianVote_clicked()
{
    CustodianVoteDialog dlg(this);
    dlg.setModel(model);
    dlg.exec();
}

void VotePage::on_motionVote_clicked()
{
    MotionVoteDialog dlg(this);
    dlg.setModel(model);
    dlg.exec();
}

void VotePage::on_feeVote_clicked()
{
    FeeVoteDialog dlg(this);
    dlg.setModel(model);
    dlg.exec();
}

void VotePage::on_reputationVote_clicked()
{
    ReputationVoteDialog dlg(this);
    dlg.setModel(model);
    dlg.exec();
}

void VotePage::on_dataFeedButton_clicked()
{
    DataFeedDialog dlg(this);
    dlg.setModel(model);
    dlg.exec();
}

void VotePage::update()
{
    if (!model)
        return;

    {
        TRY_LOCK(cs_main, lockMain);
        if (!lockMain)
            return;

        if (pindexBest == lastBestBlock)
            return;

        lastBestBlock = pindexBest;

        fillCustodianTable();
    }
}

void VotePage::fillCustodianTable()
{
    QTableWidget* table = ui->custodianTable;
    table->setRowCount(0);
    int row = 0;
    for (CBlockIndex* pindex = lastBestBlock; pindex; pindex = pindex->pprev)
    {
        BOOST_FOREACH(const CCustodianVote& custodianVote, pindex->vElectedCustodian)
        {
            table->setRowCount(row + 1);

            QTableWidgetItem *addressItem = new QTableWidgetItem();
            addressItem->setData(Qt::DisplayRole, QString::fromStdString(custodianVote.GetAddress().ToString()));
            table->setItem(row, 0, addressItem);

            QTableWidgetItem *amountItem = new QTableWidgetItem();
            amountItem->setData(Qt::DisplayRole, BitcoinUnits::format(model->getOptionsModel()->getDisplayUnit(), custodianVote.nAmount));
            amountItem->setData(Qt::TextAlignmentRole, QVariant(Qt::AlignRight | Qt::AlignVCenter));
            table->setItem(row, 1, amountItem);

            QTableWidgetItem *dateItem = new QTableWidgetItem();
            dateItem->setData(Qt::DisplayRole, GUIUtil::dateTimeStr(pindex->nTime));
            table->setItem(row, 2, dateItem);

            row++;
        }
    }
    table->setVisible(false);
    table->resizeColumnsToContents();
    table->setVisible(true);
}
