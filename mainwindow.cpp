#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "walletwidget.h"
#include "minerwidget.h"
#include "common.h"
#include "minerthread.h"

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
        showMessageBox("Can't find lavad,please select one.");
    }
    minerPath = QDir::currentPath() + "/lava-miner.exe";
    cfgPath = QDir::currentPath() + "/miner.conf";
    if (QFile(minerPath).exists() && QFile(cliPath).exists()) {
        ui->lineEdit_2->setText(minerPath);
    } else {
        showMessageBox("Can't find miner,please select one.");
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
