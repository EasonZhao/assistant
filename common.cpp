#include "common.h"
#include <qprocess.h>
#include <qdebug.h>
#include <qjsondocument.h>

const QStringList WalletParams({ "-testnet", "-rpcuser=test", "-rpcpassword=test" });

QString GetAddressPlotID(const QString program, const QString addr)
{
    QStringList arguments(WalletParams);
    arguments.append("getaddressplotid");
    arguments.append(addr);
    QProcess process;
    process.start(program, arguments);
    process.waitForFinished();
    auto ba = process.readAllStandardOutput();
    QJsonParseError json_error;
    QJsonDocument doc(QJsonDocument::fromJson(ba, &json_error));
    if (doc.isNull()) return "";
    qDebug() << doc["plotid"].type();
    if (doc["plotid"].isDouble()) {
        return QString::number(doc["plotid"].toDouble(), '.', 0);
    }
    return "";
}
