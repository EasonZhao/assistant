#ifndef LAVAHELPER_H
#define LAVAHELPER_H

#include <QObject>
#include <QJsonArray>

class LavaHelper
{
  public:
    LavaHelper(const QString& cliPath);
    double getBalance();
    int getBlockCount();
    QJsonObject getSlotInfo(const int index = -1);
    QJsonArray getFirestone(const QString addr);
  protected:
    QString execCliAndGetOutput(const QStringList& args);

  private:
    QString _cliPath;
};

#endif // LAVAHELPER_H
