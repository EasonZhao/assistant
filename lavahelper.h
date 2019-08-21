#ifndef LAVAHELPER_H
#define LAVAHELPER_H

#include <QObject>

class LavaHelper
{
  public:
    LavaHelper(const QString& cliPath);
    double getBalance();

  protected:
    QString execCliAndGetOutput(const QStringList& args);

  private:
    QString _cliPath;
};

#endif // LAVAHELPER_H
