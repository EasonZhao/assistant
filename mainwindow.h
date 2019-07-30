#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class WalletWidget;
class MinerWidget;
class MinerThread;
class QCloseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startStopWallet();

    void onCheckAddress();

    void onStartMiner();

    void onAddDir();

    void onSearchAddress();

private:
    void loadWallet();

    void rewriteConfigure();

    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    WalletWidget *wallet;
    MinerWidget *miner;
    QString cliPath;
    QString minerPath;
    QString cfgPath;
    MinerThread *thd;
};

#endif // MAINWINDOW_H
