#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class WalletWidget;
class MinerWidget;

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
private:
    void loadWallet();

private:
    Ui::MainWindow *ui;
    WalletWidget *wallet;
    MinerWidget *miner;
    QString cliPath;
    QString minerPath;
    QString cfgPath;
};

#endif // MAINWINDOW_H
