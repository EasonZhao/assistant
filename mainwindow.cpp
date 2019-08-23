#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "walletwidget.h"
#include "minerwidget.h"
#include "common.h"
#include "minerthread.h"
#include "lavahelper.h"

#include <qdir.h>
#include <qdebug.h>
#include <qfile.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopServices>
#include <QTimer>

static QString version("v1.0.0");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wallet(nullptr),
    miner(nullptr),
    thd(nullptr)
{
    ui->setupUi(this);
    //load
    loadWallet();
    setWindowTitle("lava miner assistant " + version);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startStopWallet()
{
    if (!wallet) {
        wallet = new WalletWidget(ui->lineEdit->text(), cliPath);
        scheduleUpdate();
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
    _helper = new LavaHelper(cliPath, this);
    {
        connect(_helper, SIGNAL(blockCount(int)), this, SLOT(onBlockCount(int)));
    }
    if (QFile(walletPath).exists() && QFile(cliPath).exists()) {
        ui->lineEdit->setText(walletPath);
    } else {
        showMessageBox(QString::fromWCharArray(L"找不到钱包程序, 请将Lava钱包程序放在助手所在目录"));

    }
    minerPath = QDir::currentPath() + "/lava-miner.exe";
    cfgPath = QDir::currentPath() + "/miner.conf";
    if (QFile(minerPath).exists() && QFile(cliPath).exists()) {
        ui->lineEdit_2->setText(minerPath);
    } else {
        showMessageBox(QString::fromWCharArray(L"无法找到Lava的命令行程序, 请将lava-cli.exe放在助手所在目录"));
    }
    //load miner
    QFile file(cfgPath);
    if (file.open(QIODevice::ReadWrite)) {
        auto ba = file.readAll();
        file.close();
        QJsonParseError jsonError;
        QJsonDocument doc(QJsonDocument::fromJson(ba, &jsonError));
        if (QJsonParseError::NoError != jsonError.error) {
            qDebug() <<"json error, " << jsonError.errorString();
            return;
        }
        ui->lineEdit_3->setText(doc["OwnerAddr"].toString());
        for (auto path : doc["Paths"].toArray()) {
            auto text = ui->lineEdit_4->text();
            if (text != "") {
                text += ";";
            }
            text += path.toString();
            ui->lineEdit_4->setText(text);
        }
    }
}

void MainWindow::rewriteConfigure()
{
    QFile file(cfgPath);
    if (file.open(QIODevice::ReadWrite)) {
        auto ba = file.readAll();
        file.close();
        QJsonParseError jsonError;
        QJsonDocument doc(QJsonDocument::fromJson(ba, &jsonError));
        if (QJsonParseError::NoError != jsonError.error) {
            qDebug() <<"json error, " << jsonError.errorString();
            return;
        }
        qDebug() << doc["Port"].toInt() << doc["OwnerAddr"].toString() << doc["Paths"].toArray();
        //write
        auto obj = doc.object();
        obj["Port"] = 18332;
        obj["OwnerAddr"] = ui->lineEdit_3->text();
        obj["UpdaterPort"] = 18332;
        QJsonArray jarr;
        auto dirs = ui->lineEdit_4->text().split(";");
        for (auto dir : dirs) {
            jarr.append(dir);
        }
        obj["Paths"] = jarr;
        doc.setObject(obj);
        file.open(QIODevice::WriteOnly);
        file.write(doc.toJson());
        file.flush();
        file.close();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (wallet && !wallet->isHidden()) {
        wallet->close();
    }
    if (thd && thd->isRunning()) {
        thd->terminate();
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::onCheckAddress()
{
    auto plotid = GetAddressPlotID(cliPath, ui->lineEdit_3->text());
    if (plotid.isEmpty()) {
        showMessageBox("invalid address");
    } else {
       showMessageBox("plotid: " + plotid);
    }
}

void MainWindow::onStartMiner()
{
    if (GetBlockCount(cliPath) <= 0 ) {
        showMessageBox("please wait wallet start.");
        return;
    }
    if (thd == nullptr) {
        rewriteConfigure();
        thd = new MinerThread(minerPath, this);
    }
    if (!thd->isRunning()) {
        thd->start();
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

void MainWindow::onSearchAddress()
{
    QString url("http://testnet-explorer.lavatech.org/address/" + ui->lineEdit_3->text());
    QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::on_loadWalletClciked()
{
    qDebug() << "on wallet clicked";
}

void MainWindow::scheduleUpdate() {
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onUpdate);

    timer->start(5000);
}

void MainWindow::updateSlotInfo()
{
    auto info = _helper->getSlotInfo();
    ui->labIndex->setText(QString("周期：  %1").arg(info["index"].toInt()));
    ui->labPrice->setText(QString("价格：  %1lv").arg(info["price"].toDouble()/100000000));
    auto arr = _helper->getFirestone(ui->lineEdit_3->text());
    int useable{0}, immaturate{0}, overdue{0};
    for(auto iter = arr.begin(); iter != arr.end(); iter++) {
        Q_ASSERT(iter->isObject());
        auto fs = iter->toObject();
        auto st = fs["state"].toString();
        if (st=="USEABLE") {
            useable++;
        } else if (st=="IMMATURATE") {
            immaturate++;
        } else if (st=="OVERDUE") {
            overdue++;
        }
    }
    ui->labUseable->setText(QString("可用：  %1").arg(useable));
    ui->labImmature->setText(QString("未成熟：  %1").arg(immaturate));
    ui->labOverdue->setText(QString("过期：  %1").arg(immaturate));
}

void MainWindow::onUpdate() {
  qDebug() << "updating...";
  if (!_helper) {
    qDebug() << "helper not ready...";
    return;
  }

  //update height
  _helper->getBlockCountAsync();

  auto balance = _helper->getBalance();
  QString str;
  if(balance >= 0) {
    str = QString("%1 lv").arg(balance, 4, 'f', 8, ' ');
  }
  ui->leBalance->setText(str);

  updateSlotInfo();
}

void MainWindow::onBlockCount(int height)
{
    ui->leHeight->setText(QString::number(height));
}
