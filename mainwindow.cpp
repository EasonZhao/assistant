#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "walletwidget.h"
#include "minerwidget.h"
#include "common.h"

#include <qdir.h>
#include <qdebug.h>
#include <qfile.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wallet(nullptr),
    miner(nullptr)
{
    ui->setupUi(this);
    //load
    loadWallet();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startStopWallet()
{
    if (!wallet) {
        wallet = new WalletWidget(ui->lineEdit->text());
    }
    if (wallet->isHidden()) {
        wallet->show();
    }
}

void showMessageBox(QString msg) {
    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setText(msg);
    msgBox.exec();
}

void MainWindow::loadWallet()
{
    auto walletPath = QDir::currentPath() + "/lavad.exe";
    cliPath = QDir::currentPath() + "/lava-cli.exe";
    if (QFile(walletPath).exists() && QFile(cliPath).exists()) {
        ui->lineEdit->setText(walletPath);
    } else {
        showMessageBox("未找到钱包，请手动加载");
    }
    minerPath = QDir::currentPath() + "/lava-miner.exe";
    cfgPath = QDir::currentPath() + "/miner.conf";
    if (QFile(minerPath).exists() && QFile(cliPath).exists()) {
        ui->lineEdit_2->setText(minerPath);
    } else {
        showMessageBox("未找到挖矿软件，请手动加载");
    }
}

void MainWindow::onCheckAddress()
{
    auto plotid = GetAddressPlotID(cliPath, ui->lineEdit_3->text());
    if (plotid.isEmpty()) {
        showMessageBox("无效地址。");
    } else {
        showMessageBox("plotid: " + plotid);
    }
}

void MainWindow::onStartMiner()
{
    if (!miner) {
        miner = new MinerWidget(minerPath, cfgPath, ui->lineEdit_3->text(), ui->lineEdit_4->text());
    }
    if (miner->isHidden()) {
        miner->show();
    }
}

void MainWindow::onAddDir()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec()) {
        auto dir = dialog.selectedFiles();
        auto old = ui->lineEdit_4->text();
        if (old == "") {
            ui->lineEdit_4->setText(dir[0]);
        } else {
            ui->lineEdit_4->setText(old + ";" + dir[0]);
        }
    }
}
