#include "minerthread.h"

MinerThread::MinerThread(QString minerPath, QObject *parent)
    :QThread (parent), minerPath(minerPath)
{

}

void MinerThread::run()
{
    system(qPrintable(minerPath));
}
