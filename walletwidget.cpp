#include "walletwidget.h"
#include "ui_walletwidget.h"
#include "common.h"

#include <process.h>
#include <qdebug.h>

WalletWidget::WalletWidget(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WalletWidget),
    path(path)
{
    ui->setupUi(this); 
    QObject::connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(writeOutput()));
}

WalletWidget::~WalletWidget()
{
    Close();
    delete ui;
}

void WalletWidget::Close()
{
    if (process.isOpen()) {
        process.kill();
        qDebug() << "wallet close " << process.waitForFinished();
    }
}

void WalletWidget::closeEvent(QCloseEvent *)
{
    Close();
}

void WalletWidget::showEvent(QShowEvent *)
{
    process.start(path, WalletParams, QProcess::ReadOnly);
}

void WalletWidget::writeOutput()
{
    auto output = process.readAllStandardOutput();
    ui->plainTextEdit->appendPlainText(output);
}

void WalletWidget::writeError()
{

}
