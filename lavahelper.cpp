#include "lavahelper.h"
#include <qprocess.h>
#include <qdebug.h>
#include <QJsonDocument>
#include <QJsonObject>

const QStringList WalletParams({ "-testnet", "-rpcuser=test", "-rpcpassword=test" });

LavaHelper::LavaHelper(const QString& cliPath): _cliPath(cliPath)
{
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
