#include "lavahelper.h"
#include <qprocess.h>
#include <qdebug.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

const QStringList WalletParams({ "-testnet", "-rpcuser=test", "-rpcpassword=test" });

QString LavaHelper::urlStr = QString("http://test:test@127.0.0.1:18332");

LavaHelper::LavaHelper(const QString& cliPath, QObject *parent)
    : QObject (parent),
      _cliPath(cliPath),
      manager(new QNetworkAccessManager(this))
{
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));
}

double LavaHelper::getBalance() {
  QStringList arguments("getbalance");
  auto ba = execCliAndGetOutput(arguments);
  bool success = false;
  double v = ba.toDouble(&success);
  if (success) {
    return v;
  }
  return -1.0;
}

int LavaHelper::getBlockCount()
{
    QStringList arguments("getblockcount");
    auto ba = execCliAndGetOutput(arguments);
    bool success = false;
    auto v = ba.toInt(&success);
    if (success) {
      return v;
    }
    return 0;
}

QJsonObject LavaHelper::getSlotInfo(const int index)
{
    QStringList arguments("getslotinfo");
    if (index != -1 ) {
        arguments.push_back(QString::number(index));
    }
    auto ba = execCliAndGetOutput(arguments);
    QJsonDocument doc = QJsonDocument::fromJson(ba.toLatin1());
    if( doc.isNull() ){
        qDebug()<< "===> QJsonDocument："<< ba;
    }
    return doc.object();
}

QJsonArray LavaHelper::getFirestone(const QString addr)
{
    QStringList arguments{"getfirestone", addr};
    auto ba = execCliAndGetOutput(arguments);
    QJsonDocument doc = QJsonDocument::fromJson(ba.toLatin1());
    if( doc.isNull() ){
        qDebug()<< "===> QJsonDocument："<< ba;
    }
    return doc.array();
}

void LavaHelper::getBlockCountAsync()
{
    QNetworkRequest req;
    req.setRawHeader("Content-Type", "application/json");
    QUrl url(urlStr);
    req.setUrl(url);
    QJsonObject obj {
        {"jsonrpc", "1.0"},
        {"id", ""},
        {"method", "getblockcount"},
        {"params", QJsonArray{}}
    };
    manager->post(req, QJsonDocument(obj).toJson());
}

void LavaHelper::onFinished(QNetworkReply *reply)
{
    auto ba = reply->readAll();
    qDebug() << "http client reply: " << QString(ba);
    auto doc = QJsonDocument::fromJson(ba);
    if (doc.isObject()) {
        auto obj = doc.object();
        if (obj["error"].toString() == "") {
            emit blockCount(obj["result"].toInt());
        }
    }
}

QString LavaHelper::execCliAndGetOutput(const QStringList& args) {
  QStringList arguments(WalletParams);
  arguments.append(args);
  QProcess process;
  qDebug() << "exec cli " << _cliPath << " args: " << arguments;
  process.start(_cliPath, arguments);
  process.waitForFinished();
  auto ba = process.readAllStandardOutput();
  qDebug() << "got output: " << ba;
  return ba;
}
