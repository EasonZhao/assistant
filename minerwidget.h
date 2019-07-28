#ifndef MINERWIDGET_H
#define MINERWIDGET_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class MinerWidget;
}

class MinerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MinerWidget(const QString minerPath, const QString cfgPath, const QString addr, const QString dirs, QWidget *parent = nullptr);
    ~MinerWidget();

private:
    void rewriteConfigure();

    void close();

    void closeEvent(QCloseEvent *event);

    void showEvent(QShowEvent *event);

private slots:
    void onOutput();

    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    Ui::MinerWidget *ui;
    QString minerPath;
    QString cfgPath;
    QString addr;
    QStringList dirs;
    QProcess process;
};

#endif // MINERWIDGET_H
