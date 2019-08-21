#include "lavahelper.h"
#include <qprocess.h>
#include <qdebug.h>

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
