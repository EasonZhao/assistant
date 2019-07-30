#ifndef MINERTHREAD_H
#define MINERTHREAD_H

#include <QThread>

class MinerThread : public QThread
{
public:
    explicit MinerThread(QString minerPath, QObject *parent = nullptr);

private:
    void run();

private:
    QString minerPath;
};

#endif // MINERTHREAD_H
