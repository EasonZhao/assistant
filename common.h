#ifndef COMMON_H
#define COMMON_H

#include <qstring.h>
#include <qstringlist.h>

extern const QStringList WalletParams;

QString GetAddressPlotID(const QString path, const QString addr);

#endif // COMMON_H
