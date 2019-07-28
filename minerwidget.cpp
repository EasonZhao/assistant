#include "minerwidget.h"
#include "ui_minerwidget.h"

#include <qjsondocument.h>
#include <qjsonobject.h>
#include <QJsonArray>
#include <qdebug.h>
#include <qfile.h>
#include <QDir>

MinerWidget::MinerWidget(const QString minerPath, const QString cfgPath, const QString addr, const QString dirs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MinerWidget),
    minerPath(minerPath),
    cfgPath(cfgPath),
    addr(addr)
{
    this->dirs = dirs.split(";");
    ui->setupUi(this);
    rewriteConfigure();

    QObject::connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(onOutput()));
    QObject::connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFinished(int, QProcess::ExitStatus)));
}

MinerWidget::~MinerWidget()
{
    close();
    delete ui;
}

void MinerWidget::rewriteConfigure()
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
        obj["Port"] = 18832;
        obj["OwnerAddr"] = addr;
        obj["UpdaterPort"] = 18832;
        QJsonArray jarr;
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

void MinerWidget::close()
{
    if (process.isOpen()) {
        process.kill();
        qDebug() << "miner close " << process.waitForFinished();
    }
}

void MinerWidget::closeEvent(QCloseEvent *event)
{
    close();
    QWidget::closeEvent(event);
}

void MinerWidget::showEvent(QShowEvent *event)
{

    process.setWorkingDirectory( QDir::currentPath());
    process.start(minerPath, QProcess::ReadOnly);
    QWidget::showEvent(event);
}

void MinerWidget::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << exitCode << exitStatus;
}

void MinerWidget::onOutput()
{
    auto output = process.readAllStandardOutput();
    ui->plainTextEdit->appendPlainText(output);
}
