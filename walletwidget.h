#ifndef WALLETWIDGET_H
#define WALLETWIDGET_H

#include <QWidget>
#include <qprocess.h>

namespace Ui {
class WalletWidget;
}

class QCloseEvent;
class QShowEvent;

class WalletWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WalletWidget(QString path, QWidget *parent = nullptr);
    ~WalletWidget();

    void Close();

    void closeEvent(QCloseEvent *event);

    void showEvent(QShowEvent *event);

private slots:
    void writeOutput();

    void writeError();

private:
    Ui::WalletWidget *ui;
    QProcess process;
    QString path;
};

#endif // WALLETWIDGET_H
