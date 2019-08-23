#ifndef LAVAHELPER_H
#define LAVAHELPER_H

#include <QObject>
#include <QJsonArray>

class QNetworkReply;
class QNetworkAccessManager;

class LavaHelper:public QObject
{
    Q_OBJECT
  public:
    LavaHelper(const QString& cliPath, QObject *parent = nullptr);
    double getBalance();
    int getBlockCount();
    QJsonObject getSlotInfo(const int index = -1);
    QJsonArray getFirestone(const QString addr);

    void getBlockCountAsync();

  signals:
    void blockCount(int);

  private slots:
    void onFinished(QNetworkReply *reply);

  protected:
    QString execCliAndGetOutput(const QStringList& args);

  private:
    QString _cliPath;
    QNetworkAccessManager *manager;
    static QString urlStr;
};

#endif // LAVAHELPER_H
